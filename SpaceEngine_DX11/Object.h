#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <d3d11.h>
#include <d3dcompiler.h>
#include "DirectXMath.h"
#include <fstream>

using namespace DirectX;
using namespace std;

class Object
{
public:
	Object();
	~Object();
	void Update();
	XMFLOAT3* GetPosition() { return &mPosition; }
	XMFLOAT3* GetRotation() { return &mRotation; }
	XMFLOAT3* GetScale() { return &mScale; }

protected:
	XMFLOAT3 mPosition;
	XMFLOAT3 mRotation;
	XMFLOAT3 mScale;

	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mPositionMatrix;
	XMFLOAT4X4 mRotationMatrix;
	XMFLOAT4X4 mScaleMatrix;

	//ID3D11VertexShader* m_vertexShader;
	//ID3D11PixelShader* m_pixelShader;

};
#endif