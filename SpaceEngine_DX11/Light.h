#ifndef _LIGHT_H_
#define _LIGHT_H_

#include <DirectXMath.h>
#include "Object.h"

using namespace DirectX;

class Light : public Object
{
public:
	Light();
	Light(const Light&);
	~Light();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetDirection(float, float, float);

	XMFLOAT4* GetAmbientColor();
	XMFLOAT4* GetDiffuseColor();
	XMFLOAT3* GetDirection();

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT3 m_direction;
};

#endif