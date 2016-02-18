#ifndef _Terrain_H_
#define _Terrain_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <stdio.h>
#include "Texture.h"
#include "PerlinNoise.h"
#include "AntTweakBar.h"

using namespace DirectX;

const int TEXTURE_REPEAT = 8;

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

	bool Initialize(ID3D11Device*, PerlinNoise*, WCHAR*, XMFLOAT3, int, int);
	void Shutdown();
	void Render(ID3D11DeviceContext*);
	bool UpdateBuffers(ID3D11DeviceContext* deviceContext);

	int GetIndexCount();
	int GetVertexCount();
	void CopyVertexArray(void*);
	ID3D11ShaderResourceView* GetTexture();

	XMFLOAT3* GetPosition();
	int* GetSeed() { return &mSeed; }
	int* GetTerrainHeight() { return &mTerrainHeight; }
	float* GetNoiseSize() { return &mNoiseSize; }
	float* GetPersistence() { return &mPersistence; }
	int* GetOctaves() { return &mOctaves; }
	void SetFrameTime(float time);
	bool NeedsToUpdate();
	

private:
	void NormalizeHeightMap();
	bool CalculateNormals();
	void ShutdownHeightMap();
	void GenerateFractalTerrain();

	void CalculateTextureCoordinates();
	bool LoadTexture(ID3D11Device*, WCHAR*);
	void ReleaseTexture();

	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);

	void MakeSpherical(VertexType* v);

private:
	int mTerrainSize;
	int m_vertexCount, m_indexCount;

	XMFLOAT3 mPosition;

	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	VertexType* m_vertices;
	PerlinNoise* m_PerlinNoise;
	HeightMapType* m_heightMap;
	Texture* m_Texture;

	int mSeed;
	int mTerrainHeight;
	float mNoiseSize;
	float mPersistence;
	int mOctaves;
};

#endif