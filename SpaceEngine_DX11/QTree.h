#ifndef _QTREE_H_
#define _QTREE_H_

#define LOD_LEVELS = 8;
const int MAX_TRIANGLES = 10000;

#include "Terrain.h"
#include "Frustum.h"
#include "ShaderTerrain.h"
#include <vector>

class QTree
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

	struct NodeType
	{
		float positionX, positionZ, width;
		int triangleCount;
		ID3D11Buffer *vertexBuffer, *indexBuffer;
		NodeType* nodes[4];
	};

public:
	QTree();
	QTree(const QTree&);
	~QTree();

	bool Initialize(Terrain*, ID3D11Device*);
	void Shutdown();
	void Render(Frustum*, ID3D11DeviceContext*, ShaderTerrain*);

	int GetDrawCount();

private:
	void CalculateMeshDimensions(int, float&, float&, float&);
	void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*);
	int CountTriangles(float, float, float);
	bool IsTriangleContained(int, float, float, float);
	void ReleaseNode(NodeType*);
	void RenderNode(NodeType*, Frustum*, ID3D11DeviceContext*, ShaderTerrain*);

private:
	int m_triangleCount, m_drawCount;
	VertexType* m_vertexList;
	NodeType* m_parentNode;
};

#endif