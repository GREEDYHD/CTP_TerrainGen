#ifndef _SHADERTERRAIN_H_
#define _SHADERTERRAIN_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <D3DCompiler.h>
#include <fstream>

using namespace std;
using namespace DirectX;

class ShaderTerrain
{
private:
	struct MatrixBufferType
	{
		XMFLOAT4X4 world;
		XMFLOAT4X4 view;
		XMFLOAT4X4 projection;
	};

	struct LightBufferType
	{
		XMFLOAT4 ambientColor;
		XMFLOAT4 diffuseColor;
		XMFLOAT3 lightDirection;
		float padding;
	};

public:
	ShaderTerrain();
	ShaderTerrain(const ShaderTerrain&);
	~ShaderTerrain();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4, XMFLOAT4, XMFLOAT3, ID3D11ShaderResourceView*);

	void RenderShader(ID3D11DeviceContext*, int);
	bool SetShaderParameters(ID3D11DeviceContext*, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4X4, XMFLOAT4, XMFLOAT4, XMFLOAT3, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, WCHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, WCHAR*);


private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
};

#endif