#include "Object.h"
#include <string>
#include "Timer.h"

using namespace DirectX;

Object::Object()
{
	mPosition = XMFLOAT3(0, 0, 0);
	mRotation = XMFLOAT3(0, 0, 0);
	mScale = XMFLOAT3(1, 1, 1);
}

Object::~Object()
{

}

void Object::Update()
{
	XMStoreFloat4x4(&mPositionMatrix, XMMatrixRotationRollPitchYaw(mPosition.x, mPosition.y, mPosition.z));
	XMStoreFloat4x4(&mRotationMatrix, XMMatrixRotationRollPitchYaw(mRotation.x, mRotation.y, mRotation.z));
	XMStoreFloat4x4(&mScaleMatrix, XMMatrixRotationRollPitchYaw(mScale.x, mScale.y, mScale.z));

	XMStoreFloat4x4(&mWorldMatrix, XMLoadFloat4x4(&mPositionMatrix) * XMLoadFloat4x4(&mRotationMatrix) * XMLoadFloat4x4(&mScaleMatrix));
}

//Object::AddVariableToGUI()
//{
//	
//}