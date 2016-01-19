#ifndef _Terrain_H_
#define _Terrain_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <stdio.h>
#include "Texture.h"
#include "PerlinNoise.h"

using namespace DirectX;

const int TEXTURE_REPEAT = 1;

class Terrain
{
private:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct HeightMapType
	{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	struct VectorType
	{
		float x, y, z;
	};

public:
	Terrain();
	Terrain(const Terrain&);
	~Terrain();

	bool Initialize(ID3D11Device*, PerlinNoise*, char*, WCHAR*, float, float, float, int, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	int GetVertexCount();
	void CopyVertexArray(void*);
	ID3D11ShaderResourceView* GetTexture();

private:
	bool LoadHeightMap(char*);
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();
	void ConstructGrid();
	void GenerateFractalTerrain(int, float, float, int);

	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

private:
	int m_terrainWidth, m_terrainHeight;
	int m_vertexCount, m_indexCount;

	float m_positionX, m_positionY, m_positionZ;

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	VertexType* m_vertices;
	PerlinNoise* m_PerlinNoise;
	HeightMapType* m_heightMap;
	Texture* m_Texture;
};

#endif