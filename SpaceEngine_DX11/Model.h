#ifndef _Model_H_
#define _Model_H_

#include <d3d11.h>
#include <directxmath.h>
#include "Texture.h"

using namespace DirectX;

class Model
{
private:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 tex;
		XMFLOAT3 normal;
	};

public:
	Model();
	Model(const Model&);
	~Model();

	bool Initialize(ID3D11Device*,ID3D11DeviceContext*, WCHAR*);
	void Shutdown();
	void Render(ID3D11DeviceContext*);

	int GetIndexCount();
	ID3D11ShaderResourceView* GetTexture();

private:
	bool InitializeBuffers(ID3D11Device*);
	void ShutdownBuffers();
	void RenderBuffers(ID3D11DeviceContext*);
	bool LoadTexture(ID3D11Device*, ID3D11DeviceContext*, WCHAR*);
	void ReleaseTexture();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;
	Texture* m_Texture;
};

#endif