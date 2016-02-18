#include "Terrain.h"

Terrain::Terrain()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
	m_heightMap = 0;
	m_vertices = 0;
	m_Texture = 0;

	mSeed = 100;
	mTerrainHeight = 10;
	mNoiseSize = 400;
	mPersistence = 0.5f;
	mOctaves = 8;
	mPosition = XMFLOAT3(0, 0, 0);
}

Terrain::Terrain(const Terrain& other)
{

}

Terrain::~Terrain()
{

}

bool Terrain::Initialize(ID3D11Device* device, PerlinNoise* perlinNoise, WCHAR* textureFilename, XMFLOAT3 pos, int size, int maxOctaves)
{
	bool result;

	m_PerlinNoise = perlinNoise;

	mTerrainSize = size + 1;

	m_heightMap = new HeightMapType[mTerrainSize * mTerrainSize];

	mNoiseSize = size;
	mOctaves = maxOctaves;

	mPosition.x = pos.x;
	mPosition.y = pos.y;
	mPosition.z = pos.z;

	GenerateFractalTerrain();
	//NormalizeHeightMap();

	result = CalculateNormals();
	if (!result)
	{
		return false;
	}

	//CalculateTextureCoordinates();

	// Load the texture.
	result = LoadTexture(device, textureFilename);
	if (!result)
	{
		return false;
	}

	// Initialize the vertex and index buffer that hold the geometry for the terrain.
	result = InitializeBuffers(device);
	if (!result)
	{
		return false;
	}

	ShutdownHeightMap();

	return true;
}

void Terrain::Shutdown()
{
	ReleaseTexture();
	// Release the vertex and index buffer.
	ShutdownBuffers();
	// Release the height map data.
	ShutdownHeightMap();

	return;
}

void Terrain::Render(ID3D11DeviceContext* deviceContext)
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	//if (NeedsToUpdate())
	{
		//	UpdateBuffers(deviceContext);
	}

	RenderBuffers(deviceContext);

	return;
}

int Terrain::GetIndexCount()
{
	return m_indexCount;
}

ID3D11ShaderResourceView* Terrain::GetTexture()
{
	return m_Texture->GetTexture();
}

void Terrain::GenerateFractalTerrain()
{
	//int newSize = mNoiseSize / 2;
	float trueSize = (512 / (float)pow(2, mOctaves)) / (mTerrainSize - 1);

	for (int z = 0; z < mTerrainSize; z++)
	{
		for (int x = 0; x < mTerrainSize; x++)
		{
			float total = 0.0f;

			float newX = ((mPosition.x) + (float)((x - (mTerrainSize / 2)) * trueSize));
			float newZ = ((mPosition.z) + (float)((z - (mTerrainSize / 2)) * trueSize));

			for (int i = 0; i < mOctaves; i++)
			{
				double f = (mTerrainHeight) *  m_PerlinNoise->PerlinNoise2D(mSeed, mPersistence, i, newX, newZ);
				total += (float)f;
			}

			m_heightMap[(z * mTerrainSize) + x].x = (float)(mPosition.x) + (float)((x - (mTerrainSize / 2)) * trueSize);
			m_heightMap[(z * mTerrainSize) + x].y = (float)(mPosition.y + total);
			m_heightMap[(z * mTerrainSize) + x].z = (float)(mPosition.z) + (float)((z - (mTerrainSize / 2)) * trueSize);
		}
	}
}

void Terrain::NormalizeHeightMap()
{
	int i, j;

	for (j = 0; j < mTerrainSize; j++)
	{
		for (i = 0; i < mTerrainSize; i++)
		{
			m_heightMap[(mTerrainSize * j) + i].y /= 15.0f;
		}
	}

	return;
}

bool Terrain::CalculateNormals()
{
	int i, j, index1, index2, index3, index, count;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the un-normalized normal vectors.
	normals = new VectorType[(mTerrainSize - 1) * (mTerrainSize - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j < (mTerrainSize - 1); j++)
	{
		for (i = 0; i < (mTerrainSize - 1); i++)
		{
			index1 = (j * mTerrainSize) + i;
			index2 = (j * mTerrainSize) + (i + 1);
			index3 = ((j + 1) * mTerrainSize) + i;

			// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (mTerrainSize - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);
		}
	}

	// Now go through all the vertices and take an average of each face normal 	
	// that the vertex touches to get the averaged normal for that vertex.
	for (j = 0; j < mTerrainSize; j++)
	{
		for (i = 0; i < mTerrainSize; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Initialize the count.
			count = 0;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (mTerrainSize - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Bottom right face.
			if ((i < (mTerrainSize - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (mTerrainSize - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j < (mTerrainSize - 1)))
			{
				index = (j * (mTerrainSize - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Upper right face.
			if ((i < (mTerrainSize - 1)) && (j < (mTerrainSize - 1)))
			{
				index = (j * (mTerrainSize - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
				count++;
			}

			// Take the average of the faces touching this vertex.
			sum[0] = (sum[0] / (float)count);
			sum[1] = (sum[1] / (float)count);
			sum[2] = (sum[2] / (float)count);

			// Calculate the length of this normal.
			length = sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * mTerrainSize) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

void Terrain::ShutdownHeightMap()
{
	if (m_heightMap)
	{
		delete[] m_heightMap;
		m_heightMap = 0;
	}

	return;
}

void Terrain::CalculateTextureCoordinates()
{
	int incrementCount, i, j, tuCount, tvCount;
	float incrementValue, tuCoordinate, tvCoordinate;


	// Calculate how much to increment the texture coordinates by.
	incrementValue = (float)TEXTURE_REPEAT / (float)mTerrainSize;

	// Calculate how many times to repeat the texture.
	incrementCount = mTerrainSize / TEXTURE_REPEAT;

	// Initialize the tu and tv coordinate values.
	tuCoordinate = 0.0f;
	tvCoordinate = 1.0f;

	// Initialize the tu and tv coordinate indexes.
	tuCount = 0;
	tvCount = 0;

	// Loop through the entire height map and calculate the tu and tv texture coordinates for each vertex.
	for (j = 0; j < mTerrainSize; j++)
	{
		for (i = 0; i < mTerrainSize; i++)
		{
			// Store the texture coordinate in the height map.
			m_heightMap[(mTerrainSize * j) + i].tu = tuCoordinate;
			m_heightMap[(mTerrainSize * j) + i].tv = tvCoordinate;

			// Increment the tu texture coordinate by the increment value and increment the index by one.
			tuCoordinate += incrementValue;
			tuCount++;

			// Check if at the far right end of the texture and if so then start at the beginning again.
			if (tuCount == incrementCount)
			{
				tuCoordinate = 0.0f;
				tuCount = 0;
			}
		}

		// Increment the tv texture coordinate by the increment value and increment the index by one.
		tvCoordinate -= incrementValue;
		tvCount++;

		// Check if at the top of the texture and if so then start at the bottom again.
		if (tvCount == incrementCount)
		{
			tvCoordinate = 1.0f;
			tvCount = 0;
		}
	}

	return;
}

bool Terrain::LoadTexture(ID3D11Device* device, WCHAR* filename)
{
	bool result;


	// Create the texture object.
	m_Texture = new Texture;
	if (!m_Texture)
	{
		return false;
	}

	// Initialize the texture object.
	result = m_Texture->Initialize(device, filename);
	if (!result)
	{
		return false;
	}

	return true;
}

void Terrain::ReleaseTexture()
{
	// Release the texture object.
	if (m_Texture)
	{
		m_Texture->Shutdown();
		delete m_Texture;
		m_Texture = 0;
	}

	return;
}

bool Terrain::InitializeBuffers(ID3D11Device* device)
{
	unsigned long* indices;
	int index, i, j;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;
	int index1, index2, index3, index4;
	float tu, tv;


	// Calculate the number of vertices in the terrain mesh.
	m_vertexCount = (mTerrainSize - 1) * (mTerrainSize - 1) * 6;

	// Set the index count to the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the vertex array.
	m_vertices = new VertexType[m_vertexCount];
	if (!m_vertices)
	{
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return false;
	}

	// Initialize the index to the vertex buffer.
	index = 0;

	// Load the vertex and index array with the terrain data.
	for (j = 0; j < (mTerrainSize - 1); j++)
	{
		for (i = 0; i < (mTerrainSize - 1); i++)
		{
			index1 = (mTerrainSize * j) + i;          // Bottom left.
			index2 = (mTerrainSize * j) + (i + 1);      // Bottom right.
			index3 = (mTerrainSize * (j + 1)) + i;      // Upper left.
			index4 = (mTerrainSize * (j + 1)) + (i + 1);  // Upper right.

															 // Upper left.
			tv = m_heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			m_vertices[index].texture = XMFLOAT2(m_heightMap[index3].tu, tv);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			m_vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Bottom left.
			m_vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			m_vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			indices[index] = index;
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			m_vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			m_vertices[index].texture = XMFLOAT2(tu, tv);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			indices[index] = index;
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if (tu == 0.0f) { tu = 1.0f; }

			m_vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			m_vertices[index].texture = XMFLOAT2(tu, m_heightMap[index2].tv);
			m_vertices[index].normal = XMFLOAT3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			indices[index] = index;
			index++;
		}
	}
	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = m_vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if (FAILED(result))
	{
		return false;
	}

	delete[] indices;
	indices = 0;
	delete[] m_vertices;
	m_vertices = 0;

	//for (size_t i = 0; i < sizeof(m_vertices); i++)
	//{
	//	MakeSpherical(&m_vertices[i]);
	//}

	return true;
}

void Terrain::MakeSpherical(VertexType* v)
{
	float x2 = (*v).position.x * (*v).position.x;
	float y2 = (*v).position.y * (*v).position.y;
	float z2 = (*v).position.z * (*v).position.z;
	XMFLOAT3 s;
	s.x = (*v).position.x * sqrtf(1.0f - y2 / 2.0f - z2 / 2.0f + y2 * z2 / 3.0f);
	s.y = (*v).position.y * sqrtf(1.0f - x2 / 2.0f - z2 / 2.0f + x2 * z2 / 3.0f);
	s.z = (*v).position.z * sqrtf(1.0f - x2 / 2.0f - y2 / 2.0f + x2 * y2 / 3.0f);
	(*v).normal = s;
	(*v).position = (*v).normal;
}

void Terrain::ShutdownBuffers()
{
	// Release the vertex array.
	if (m_vertices)
	{
		delete[] m_vertices;
		m_vertices = 0;
	}

	return;
}

int Terrain::GetVertexCount()
{
	return m_vertexCount;
}

void Terrain::CopyVertexArray(void* vertexList)
{
	memcpy(vertexList, m_vertices, sizeof(VertexType) * m_vertexCount);

	return;
}

void Terrain::RenderBuffers(ID3D11DeviceContext* deviceContext)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case a line list.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

//void Terrain::SetFrameTime(float time)
//{
//	if (m_frameTime > 100)
//	{
//		m_frameTime = 0;
//	}
//	else
//	{
//		m_frameTime += time;
//	}
//
//	return;
//}

bool Terrain::UpdateBuffers(ID3D11DeviceContext* deviceContext)
{
	VertexType* vertices;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	VertexType* verticesPtr;
	HRESULT result;

	int index;
	int index1, index2, index3, index4;
	float tu, tv;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if (!vertices)
	{
		return false;
	}

	GenerateFractalTerrain();
	CalculateNormals();

	index = 0;
	for (int j = 0; j < (mTerrainSize - 1); j++)
	{
		for (int i = 0; i < (mTerrainSize - 1); i++)
		{
			index1 = (mTerrainSize * j) + i;
			index2 = (mTerrainSize * j) + (i + 1);
			index3 = (mTerrainSize * (j + 1)) + i;
			index4 = (mTerrainSize * (j + 1)) + (i + 1);


			tv = m_heightMap[index3].tv;

			// Modify the texture coordinates to cover the top edge.
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = XMFLOAT3(m_heightMap[index3].x, m_heightMap[index3].y, m_heightMap[index3].z);
			vertices[index].texture = XMFLOAT2(m_heightMap[index3].tu, tv);
			vertices[index].normal = XMFLOAT3(m_heightMap[index3].nx, m_heightMap[index3].ny, m_heightMap[index3].nz);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = XMFLOAT2(tu, tv);
			vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			index++;

			// Bottom left.
			vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			index++;

			// Bottom left.
			vertices[index].position = XMFLOAT3(m_heightMap[index1].x, m_heightMap[index1].y, m_heightMap[index1].z);
			vertices[index].texture = XMFLOAT2(m_heightMap[index1].tu, m_heightMap[index1].tv);
			vertices[index].normal = XMFLOAT3(m_heightMap[index1].nx, m_heightMap[index1].ny, m_heightMap[index1].nz);
			index++;

			// Upper right.
			tu = m_heightMap[index4].tu;
			tv = m_heightMap[index4].tv;

			// Modify the texture coordinates to cover the top and right edge.
			if (tu == 0.0f) { tu = 1.0f; }
			if (tv == 1.0f) { tv = 0.0f; }

			vertices[index].position = XMFLOAT3(m_heightMap[index4].x, m_heightMap[index4].y, m_heightMap[index4].z);
			vertices[index].texture = XMFLOAT2(tu, tv);
			vertices[index].normal = XMFLOAT3(m_heightMap[index4].nx, m_heightMap[index4].ny, m_heightMap[index4].nz);
			index++;

			// Bottom right.
			tu = m_heightMap[index2].tu;

			// Modify the texture coordinates to cover the right edge.
			if (tu == 0.0f) { tu = 1.0f; }

			vertices[index].position = XMFLOAT3(m_heightMap[index2].x, m_heightMap[index2].y, m_heightMap[index2].z);
			vertices[index].texture = XMFLOAT2(tu, tv);
			vertices[index].normal = XMFLOAT3(m_heightMap[index2].nx, m_heightMap[index2].ny, m_heightMap[index2].nz);
			index++;
		}
	}

	// Lock the vertex buffer so it can be written to.
	result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (FAILED(result))
	{
		return false;
	}

	// Get a pointer to the data in the vertex buffer.
	verticesPtr = (VertexType*)mappedResource.pData;

	// Copy the data into the vertex buffer.
	memcpy(verticesPtr, (void*)vertices, (sizeof(VertexType) * m_vertexCount));

	// Unlock the vertex buffer.
	deviceContext->Unmap(m_vertexBuffer, 0);

	// Release the vertex array as it is no longer needed.
	delete[] vertices;
	vertices = 0;


	return true;
}

XMFLOAT3* Terrain::GetPosition()
{
	return &mPosition;
}

//bool Terrain::NeedsToUpdate()
//{
//	if (mPreviousSeed != mSeed || mPreviousTerrainHeight != mTerrainHeight || mPreviousNoiseSize != mNoiseSize || mPersistence != mPreviousPersistence || mPreviousOctaves != mOctaves)
//	{
//		mPreviousSeed = mSeed;
//		mPreviousTerrainHeight = mTerrainHeight;
//		mPreviousNoiseSize = mNoiseSize;
//		mPreviousPersistence = mPersistence;
//		mPreviousOctaves = mOctaves;
//
//		return true;
//	}
//	return false;
//}