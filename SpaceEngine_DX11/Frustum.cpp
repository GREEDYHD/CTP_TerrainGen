#include "Frustum.h"

Frustum::Frustum()
{

}

Frustum::Frustum(const Frustum& other)
{

}

Frustum::~Frustum()
{

}

void Frustum::ConstructFrustum(float screenDepth, XMFLOAT4X4 projectionMatrix, XMFLOAT4X4 viewMatrix)
{
	float zMinimum, r;
	XMFLOAT4X4 matrix;


	// Calculate the minimum Z distance in the frustum.
	zMinimum = -projectionMatrix._43 / projectionMatrix._33;
	r = screenDepth / (screenDepth - zMinimum);
	projectionMatrix._33 = r;
	projectionMatrix._43 = -r * zMinimum;

	// Create the frustum matrix from the view matrix and updated projection matrix.
	XMLoadFloat4x4(&matrix) = XMMatrixMultiply(XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projectionMatrix));

	// Calculate near plane of frustum.
	m_planes[0].x = matrix._14 + matrix._13;
	m_planes[0].y = matrix._24 + matrix._23;
	m_planes[0].z = matrix._34 + matrix._33;
	m_planes[0].w = matrix._44 + matrix._43;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[0]));

	// Calculate far plane of frustum.
	m_planes[1].x = matrix._14 - matrix._13;
	m_planes[1].y = matrix._24 - matrix._23;
	m_planes[1].z = matrix._34 - matrix._33;
	m_planes[1].w = matrix._44 - matrix._43;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[1]));

	// Calculate left plane of frustum.
	m_planes[2].x = matrix._14 + matrix._11;
	m_planes[2].y = matrix._24 + matrix._21;
	m_planes[2].z = matrix._34 + matrix._31;
	m_planes[2].w = matrix._44 + matrix._41;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[2]));

	// Calculate right plane of frustum.
	m_planes[3].x = matrix._14 - matrix._11;
	m_planes[3].y = matrix._24 - matrix._21;
	m_planes[3].z = matrix._34 - matrix._31;
	m_planes[3].w = matrix._44 - matrix._41;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[3]));

	// Calculate top plane of frustum.
	m_planes[4].x = matrix._14 - matrix._12;
	m_planes[4].y = matrix._24 - matrix._22;
	m_planes[4].z = matrix._34 - matrix._32;
	m_planes[4].w = matrix._44 - matrix._42;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[4]));

	// Calculate bottom plane of frustum.
	m_planes[5].x = matrix._14 + matrix._12;
	m_planes[5].y = matrix._24 + matrix._22;
	m_planes[5].z = matrix._34 + matrix._32;
	m_planes[5].w = matrix._44 + matrix._42;
	XMPlaneNormalize(XMLoadFloat4(&m_planes[5]));

	return;
}


bool Frustum::CheckPoint(float x, float y, float z)
{
	int i;

	// Check if the point is inside all six planes of the view frustum.
	for (i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4(x, y, z, 1.0f))).m128_f32[0] < 0.0f)
		{
			return false;
		}
	}

	return true;
}


bool Frustum::CheckCube(float xCenter, float yCenter, float zCenter, float radius)
{
	// Check if any one point of the cube is in the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter - radius), (zCenter - radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter - radius), (zCenter - radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter + radius), (zCenter - radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter + radius), (zCenter - radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter - radius), (zCenter + radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter - radius), (zCenter + radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - radius), (yCenter + radius), (zCenter + radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + radius), (yCenter + radius), (zCenter + radius), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}

bool Frustum::CheckSphere(float xCenter, float yCenter, float zCenter, float radius)
{
	// Check if the radius of the sphere is inside the view frustum.
	for (int i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4(xCenter, yCenter, zCenter, 1.0f))).m128_f32[0] < -radius)
		{
			return false;
		}
	}

	return true;
}


bool Frustum::CheckRectangle(float xCenter, float yCenter, float zCenter, float xSize, float ySize, float zSize)
{
	int i;


	// Check if any of the 6 planes of the rectangle are inside the view frustum.
	for (i = 0; i < 6; i++)
	{
		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter - ySize), (zCenter - zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter + ySize), (zCenter - zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter - ySize), (zCenter + zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter - xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		if (XMPlaneDotCoord(XMLoadFloat4(&m_planes[i]), XMLoadFloat4(&XMFLOAT4((xCenter + xSize), (yCenter + ySize), (zCenter + zSize), 1.0f))).m128_f32[0] >= 0.0f)
		{
			continue;
		}

		return false;
	}

	return true;
}