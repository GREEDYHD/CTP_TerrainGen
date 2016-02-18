#include "QuadTree.h"
#include <Windows.h>
#include <iostream>
#include <sstream>

#define DBOUT( s )            \
{                             \
   std::wostringstream os_;    \
   os_ << s;                   \
   OutputDebugStringW( os_.str().c_str() );  \
}

//QuadTree::QuadTree()
//{
//	m_vertexList = 0;
//	mRootNode = 0;
//}
//
//QuadTree::QuadTree(const QuadTree& other)
//{
//
//}
//
//QuadTree::~QuadTree()
//{
//
//}
//
//bool QuadTree::Initialize(Terrain* terrain, ID3D11Device* device)
//{
//	int vertexCount;
//	float centerX, centerZ, width;
//
//	// Get the number of vertices in the terrain vertex array.
//	vertexCount = terrain->GetVertexCount();
//
//	// Store the total triangle count for the vertex list.
//	m_triangleCount = vertexCount / 3;
//
//	// Create a vertex array to hold all of the terrain vertices.
//	m_vertexList = new VertexType[vertexCount];
//	if (!m_vertexList)
//	{
//		return false;
//	}
//
//	// Copy the terrain vertices into the vertex list.
//	terrain->CopyVertexArray((void*)m_vertexList);
//
//	// Calculate the center x,z and the width of the mesh.
//	CalculateMeshDimensions(vertexCount, centerX, centerZ, width);
//
//	// Create the parent node for the quad tree.
//	mRootNode = new NodeType;
//	if (!mRootNode)
//	{
//		return false;
//	}
//
//	// Recursively build the quad tree based on the vertex list data and mesh dimensions.
//	CreateTreeNode(mRootNode, centerX, centerZ, width, device, 0);
//
//	// Release the vertex list since the quad tree now has the vertices in each node.
//	if (m_vertexList)
//	{
//		delete[] m_vertexList;
//		m_vertexList = 0;
//	}
//
//	return true;
//}
//
//
//void QuadTree::Shutdown()
//{
//	// Recursively release the quad tree data.
//	if (mRootNode)
//	{
//		ReleaseNode(mRootNode);
//		delete mRootNode;
//		mRootNode = 0;
//	}
//
//	return;
//}
//
//void QuadTree::Render(XMFLOAT3* pos, Frustum* frustum, ID3D11DeviceContext* deviceContext, ShaderTerrain* shader)
//{
//	// Reset the number of triangles that are drawn for this frame.
//	m_drawCount = 0;
//
//	// Render each node that is visible starting at the parent node and moving down the tree.
//	RenderNode(pos, mRootNode, frustum, deviceContext, shader);
//
//	return;
//}
//
//int QuadTree::GetDrawCount()
//{
//	return m_drawCount;
//}
//
//void QuadTree::CalculateMeshDimensions(int vertexCount, float& centerX, float& centerZ, float& meshWidth)
//{
//	int i;
//	float maxWidth, maxDepth, minWidth, minDepth, width, depth, maxX, maxZ;
//
//
//	// Initialize the center position of the mesh to zero.
//	centerX = 0.0f;
//	centerZ = 0.0f;
//
//	// Sum all the vertices in the mesh.
//	for (i = 0; i < vertexCount; i++)
//	{
//		centerX += m_vertexList[i].position.x;
//		centerZ += m_vertexList[i].position.z;
//	}
//
//	// And then divide it by the number of vertices to find the mid-point of the mesh.
//	centerX = centerX / (float)vertexCount;
//	centerZ = centerZ / (float)vertexCount;
//
//	// Initialize the maximum and minimum size of the mesh.
//	maxWidth = 0.0f;
//	maxDepth = 0.0f;
//
//	minWidth = fabsf(m_vertexList[0].position.x - centerX);
//	minDepth = fabsf(m_vertexList[0].position.z - centerZ);
//
//	// Go through all the vertices and find the maximum and minimum width and depth of the mesh.
//	for (i = 0; i < vertexCount; i++)
//	{
//		width = fabsf(m_vertexList[i].position.x - centerX);
//		depth = fabsf(m_vertexList[i].position.z - centerZ);
//
//		if (width > maxWidth) { maxWidth = width; }
//		if (depth > maxDepth) { maxDepth = depth; }
//		if (width < minWidth) { minWidth = width; }
//		if (depth < minDepth) { minDepth = depth; }
//	}
//
//	// Find the absolute maximum value between the min and max depth and width.
//	maxX = (float)max(fabs(minWidth), fabs(maxWidth));
//	maxZ = (float)max(fabs(minDepth), fabs(maxDepth));
//
//	// Calculate the maximum diameter of the mesh.
//	meshWidth = max(maxX, maxZ) * 2.0f;
//
//	return;
//}
//
//void QuadTree::CreateTreeNode(NodeType* node, float positionX, float positionZ, float width, ID3D11Device* device, int lodLevel)
//{
//	int numTriangles, i, count, vertexCount, index, vertexIndex;
//	float offsetX, offsetZ;
//	VertexType* vertices;
//	unsigned long* indices;
//	bool result;
//	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
//	D3D11_SUBRESOURCE_DATA vertexData, indexData;
//
//	// Store the node position and size.
//	node->positionX = positionX;
//	node->positionZ = positionZ;
//	node->width = width;
//
//	// Initialize the triangle count to zero for the node.
//	node->triangleCount = 0;
//
//	// Initialize the vertex and index buffer to null.
//	node->vertexBuffer = 0;
//	node->indexBuffer = 0;
//
//	// Initialize the children nodes of this node to null.
//	node->nodes[0] = nullptr;
//	node->nodes[1] = nullptr;
//	node->nodes[2] = nullptr;
//	node->nodes[3] = nullptr;
//
//	// Count the number of triangles that are inside this node.
//	numTriangles = CountTriangles(positionX, positionZ, width);
//
//	// Case 1: If there are no triangles in this node then return as it is empty and requires no processing.
//	if (numTriangles == 0)
//	{
//		return;
//	}
//
//	// Case 2: If there are too many triangles in this node then split it into four equal sized smaller tree nodes.
//	if (numTriangles > MAX_TRIANGLES)
//	{
//		for (i = 0; i < 4; i++)
//		{
//			// Calculate the position offsets for the new child node.
//			offsetX = (((i % 2) < 1) ? -1.0f : 1.0f) * (width / 4.0f);
//			offsetZ = (((i % 4) < 2) ? -1.0f : 1.0f) * (width / 4.0f);
//
//			// See if there are any triangles in the new node.
//			count = CountTriangles((positionX + offsetX), (positionZ + offsetZ), (width / 2.0f));
//			if (count > 0)
//			{
//				// If there are triangles inside where this new node would be then create the child node.
//				node->nodes[i] = new NodeType;
//
//				// Extend the tree starting from this new child node now.
//				CreateTreeNode(node->nodes[i], (positionX + offsetX), (positionZ + offsetZ), (width / 2.0f), device, node->LODLevel);
//			}
//		}
//
//		return;
//	}
//
//	// Case 3: If this node is not empty and the triangle count for it is less than the max then 
//	// this node is at the bottom of the tree so create the list of triangles to store in it.
//	node->triangleCount = numTriangles;
//
//	// Calculate the number of vertices.
//	vertexCount = numTriangles * 3;
//
//	// Create the vertex array.
//	vertices = new VertexType[vertexCount];
//
//	// Create the index array.
//	indices = new unsigned long[vertexCount];
//
//	// Initialize the index for this new vertex and index array.
//	index = 0;
//
//	// Go through all the triangles in the vertex list.
//	for (i = 0; i < m_triangleCount; i++)
//	{
//		// If the triangle is inside this node then add it to the vertex array.
//		result = IsTriangleContained(i, positionX, positionZ, width);
//		if (result == true)
//		{
//			// Calculate the index into the terrain vertex list.
//			vertexIndex = i * 3;
//
//			// Get the three vertices of this triangle from the vertex list.
//			vertices[index].position = m_vertexList[vertexIndex].position;
//			vertices[index].texture = m_vertexList[vertexIndex].texture;
//			vertices[index].normal = m_vertexList[vertexIndex].normal;
//			indices[index] = index;
//			index++;
//
//			vertexIndex++;
//			vertices[index].position = m_vertexList[vertexIndex].position;
//			vertices[index].texture = m_vertexList[vertexIndex].texture;
//			vertices[index].normal = m_vertexList[vertexIndex].normal;
//			indices[index] = index;
//			index++;
//
//			vertexIndex++;
//			vertices[index].position = m_vertexList[vertexIndex].position;
//			vertices[index].texture = m_vertexList[vertexIndex].texture;
//			vertices[index].normal = m_vertexList[vertexIndex].normal;
//			indices[index] = index;
//			index++;
//		}
//	}
//
//	// Set up the description of the vertex buffer.
//	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
//	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
//	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
//	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	vertexBufferDesc.MiscFlags = 0;
//	vertexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the vertex data.
//	vertexData.pSysMem = vertices;
//	vertexData.SysMemPitch = 0;
//	vertexData.SysMemSlicePitch = 0;
//
//	// Now finally create the vertex buffer.
//	device->CreateBuffer(&vertexBufferDesc, &vertexData, &node->vertexBuffer);
//
//	// Set up the description of the index buffer.
//	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
//	indexBufferDesc.ByteWidth = sizeof(unsigned long) * vertexCount;
//	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	indexBufferDesc.CPUAccessFlags = 0;
//	indexBufferDesc.MiscFlags = 0;
//	indexBufferDesc.StructureByteStride = 0;
//
//	// Give the subresource structure a pointer to the index data.
//	indexData.pSysMem = indices;
//	indexData.SysMemPitch = 0;
//	indexData.SysMemSlicePitch = 0;
//
//	// Create the index buffer.
//	device->CreateBuffer(&indexBufferDesc, &indexData, &node->indexBuffer);
//
//	// Release the vertex and index arrays now that the data is stored in the buffers in the node.
//	delete[] vertices;
//	vertices = 0;
//
//	delete[] indices;
//	indices = 0;
//
//	return;
//}
//
//int QuadTree::CountTriangles(float positionX, float positionZ, float width)
//{
//	int count, i;
//	bool result;
//
//
//	// Initialize the count to zero.
//	count = 0;
//
//	// Go through all the triangles in the entire mesh and check which ones should be inside this node.
//	for (i = 0; i < m_triangleCount; i++)
//	{
//		// If the triangle is inside the node then increment the count by one.
//		result = IsTriangleContained(i, positionX, positionZ, width);
//		if (result == true)
//		{
//			count++;
//		}
//	}
//
//	return count;
//}
//
//bool QuadTree::IsTriangleContained(int index, float positionX, float positionZ, float width)
//{
//	float radius;
//	int vertexIndex;
//	float x1, z1, x2, z2, x3, z3;
//	float minimumX, maximumX, minimumZ, maximumZ;
//
//
//	// Calculate the radius of this node.
//	radius = width / 2.0f;
//
//	// Get the index into the vertex list.
//	vertexIndex = index * 3;
//
//	// Get the three vertices of this triangle from the vertex list.
//	x1 = m_vertexList[vertexIndex].position.x;
//	z1 = m_vertexList[vertexIndex].position.z;
//	vertexIndex++;
//
//	x2 = m_vertexList[vertexIndex].position.x;
//	z2 = m_vertexList[vertexIndex].position.z;
//	vertexIndex++;
//
//	x3 = m_vertexList[vertexIndex].position.x;
//	z3 = m_vertexList[vertexIndex].position.z;
//
//	// Check to see if the minimum of the x coordinates of the triangle is inside the node.
//	minimumX = min(x1, min(x2, x3));
//	if (minimumX > (positionX + radius))
//	{
//		return false;
//	}
//
//	// Check to see if the maximum of the x coordinates of the triangle is inside the node.
//	maximumX = max(x1, max(x2, x3));
//	if (maximumX < (positionX - radius))
//	{
//		return false;
//	}
//
//	// Check to see if the minimum of the z coordinates of the triangle is inside the node.
//	minimumZ = min(z1, min(z2, z3));
//	if (minimumZ > (positionZ + radius))
//	{
//		return false;
//	}
//
//	// Check to see if the maximum of the z coordinates of the triangle is inside the node.
//	maximumZ = max(z1, max(z2, z3));
//	if (maximumZ < (positionZ - radius))
//	{
//		return false;
//	}
//
//	return true;
//}
//
//void QuadTree::ReleaseNode(NodeType* node)
//{
//	int i;
//
//
//	// Recursively go down the tree and release the bottom nodes first.
//	for (i = 0; i < 4; i++)
//	{
//		if (node->nodes[i] != 0)
//		{
//			ReleaseNode(node->nodes[i]);
//		}
//	}
//
//	// Release the vertex buffer for this node.
//	if (node->vertexBuffer)
//	{
//		node->vertexBuffer->Release();
//		node->vertexBuffer = 0;
//	}
//
//	// Release the index buffer for this node.
//	if (node->indexBuffer)
//	{
//		node->indexBuffer->Release();
//		node->indexBuffer = 0;
//	}
//
//	// Release the four child nodes.
//	for (i = 0; i < 4; i++)
//	{
//		if (node->nodes[i])
//		{
//			delete node->nodes[i];
//			node->nodes[i] = 0;
//		}
//	}
//
//	return;
//}
//
//void QuadTree::RenderNode(XMFLOAT3* pos, NodeType* node, Frustum* frustum, ID3D11DeviceContext* deviceContext, ShaderTerrain* shader)
//{
//	bool result;
//	int count, i, indexCount;
//	unsigned int stride, offset;
//
//	// Check to see if the node can be viewed, height doesn't matter in a quad tree.
//	result = frustum->CheckCube(node->positionX, 0.0f, node->positionZ, (node->width / 2.0f));
//
//	// If it can't be seen then none of its children can either so don't continue down the tree, this is where the speed is gained.
//	if (!result)
//	{
//		return;
//	}
//
//	// If it can be seen then check all four child nodes to see if they can also be seen.
//	count = 0;
//	for (i = 0; i < 4; i++)
//	{
//		if (node->nodes[i] != 0)
//		{
//			count++;
//			RenderNode(pos, node->nodes[i], frustum, deviceContext, shader);
//		}
//	}
//
//	// If there were any children nodes then there is no need to continue as parent nodes won't contain any triangles to render.
//	if (count != 0)
//	{
//		return;
//	}
//
//	// Set vertex buffer stride and offset.
//	stride = sizeof(VertexType);
//	offset = 0;
//
//	// Set the vertex buffer to active in the input assembler so it can be rendered.
//	deviceContext->IASetVertexBuffers(0, 1, &node->vertexBuffer, &stride, &offset);
//
//	// Set the index buffer to active in the input assembler so it can be rendered.
//	deviceContext->IASetIndexBuffer(node->indexBuffer, DXGI_FORMAT_R32_UINT, 0);
//
//	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
//	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//	// Determine the number of indices in this node.
//	indexCount = node->triangleCount * 3;
//
//	// Call the terrain shader to render the polygons in this node.
//	shader->RenderShader(deviceContext, indexCount);
//
//	// Increase the count of the number of polygons that have been rendered during this frame.
//	m_drawCount += node->triangleCount;
//
//	return;
//}

XMFLOAT3 operator+ (XMFLOAT3 a, XMFLOAT3 b)
{
	return XMFLOAT3(a.x + b.x, a.y + b.y, a.z + b.z);
}

QuadTree::QuadTree(int lvl, ID3D11Device* device, PerlinNoise* perlinNoise, XMFLOAT3 pos, int terrainSize)
{
	mTerrain = new Terrain();
	mLevel = lvl;
	mNodePosition = pos;

	if (mLevel == 0)
	{
		mTerrain->Initialize(device, perlinNoise, L"Red.dds", pos, terrainSize, lvl + 1);
	}
	if (mLevel == 1)
	{
		mTerrain->Initialize(device, perlinNoise, L"Green.dds", pos, terrainSize, lvl + 1);
	}
	if (mLevel == 2)
	{
		mTerrain->Initialize(device, perlinNoise, L"Blue.dds", pos, terrainSize, lvl + 1);
	}
	if (mLevel == 3)
	{
		mTerrain->Initialize(device, perlinNoise, L"Cyan.dds", pos, terrainSize, lvl + 1);
	}
	if (mLevel == 4)
	{
		mTerrain->Initialize(device, perlinNoise, L"Magenta.dds", pos, terrainSize, lvl + 1);
	}
	if (mLevel == 5)
	{
		mTerrain->Initialize(device, perlinNoise, L"Yellow.dds", pos, terrainSize, lvl + 1);
	}
	if (mLevel > 5)
	{
		mTerrain->Initialize(device, perlinNoise, L"TEX_Stone.dds", pos, terrainSize, lvl + 1);
	}


	//Quadtrees that exceed this size will contain no child nodes and will therfore be leaf nodes with the maximum detail possible of 256 x 256
	//if (terrainSize <= 64) // 65536 is max number of vertices allowed in a single vertex buffer 256 x 256 = 65536 therefore terrain size must not exceed this value
	XMFLOAT3 tmpPos = pos;// +XMFLOAT3(0, -10, 0);

	//float tXP = mNodePosition.x + (terrainSize / 2);
	//float tXN = mNodePosition.x - (terrainSize / 2);
	//float tZP = mNodePosition.z + (terrainSize / 2);
	//float tZN = mNodePosition.z - (terrainSize / 2);

	//float trueSize = (256.0f / ((float)lvl + 1.0f)) / ((terrainSize - 1) / (float)lvl);

	float trueSize = (256 / (float)pow(2,lvl)) / (terrainSize);

	/*if ((int)trueSize % 2 != 0)
	{
		float tXP = pos.x + (trueSize);
		float tXN = pos.x - (trueSize);
		float tZP = pos.z + (trueSize);
		float tZN = pos.z - (trueSize);
	}*/

	float tXP = pos.x + (trueSize);
	float tXN = pos.x - (trueSize);
	float tZP = pos.z + (trueSize);
	float tZN = pos.z - (trueSize);

	for (size_t i = 0; i < 4; i++)
	{
		mChildNodes[i] = nullptr;
	}
	DBOUT(tmpPos.x << "\n");
	if (lvl <= 4)
	{
		tmpPos = XMFLOAT3(tXN, 0, tZP);
		mChildNodes[0] = new QuadTree(lvl + 1, device, perlinNoise, tmpPos, terrainSize);

		tmpPos = XMFLOAT3(tXP, 0, tZP);
		mChildNodes[1] = new QuadTree(lvl + 1, device, perlinNoise, tmpPos, terrainSize);

		tmpPos = XMFLOAT3(tXP, 0, tZN);
		mChildNodes[2] = new QuadTree(lvl + 1, device, perlinNoise, tmpPos, terrainSize);

		tmpPos = XMFLOAT3(tXN, 0, tZN);
		mChildNodes[3] = new QuadTree(lvl + 1, device, perlinNoise, tmpPos, terrainSize);
	}
	return;
};

bool QuadTree::RenderTree(ID3D11DeviceContext* context, XMFLOAT3* pos)
{
	//if ((*XMVector3Length(XMLoadFloat3(mTerrain->GetPosition()) - XMLoadFloat3(pos)).m128_i32) < 2000) //Select detail level based on camera distance

	if ((*mTerrain->GetPosition()).x < 10)
	{
		for (size_t i = 0; i < 4; i++)
		{
			if (mChildNodes[i] == nullptr)
			{
				mTerrain->Render(context);
				break;
			}
		}
	}
	//else
	{

	}
	return true;
}

void QuadTree::GetVisibleTerrain(XMFLOAT3* pos, vector<Terrain*>* list) //Recursive function to traverse the tree and find the nodes that should be rendered
{
	if (mLevel == 4)
	{
		(*list).push_back(mTerrain);
	}
	else
	{
		if (mChildNodes[0] != nullptr)
		{
			mChildNodes[0]->GetVisibleTerrain(pos, list);
			mChildNodes[1]->GetVisibleTerrain(pos, list);
			mChildNodes[2]->GetVisibleTerrain(pos, list);
			mChildNodes[3]->GetVisibleTerrain(pos, list);
		}
	}
}