#ifndef _QUADTREE_H_
#define _QUADTREE_H_

const int MAX_TRIANGLES = 8192;

#include "Terrain.h"
#include "Frustum.h"
#include "ShaderTerrain.h"
#include "PerlinNoise.h"
#include <vector>

class QuadTree
{
	//private:
	//
	//	struct VertexType
	//	{
	//		XMFLOAT3 position;
	//		XMFLOAT2 texture;
	//		XMFLOAT3 normal;
	//	};
	//
	//	struct NodeType
	//	{
	//		float positionX, positionZ, width;
	//		int triangleCount;
	//		int LODLevel;
	//		ID3D11Buffer *vertexBuffer, *indexBuffer;
	//		NodeType* nodes[4];
	//	};
	//
	//public:
	//	QuadTree();
	//	QuadTree(const QuadTree&);
	//	~QuadTree();
	//
	//	bool Initialize(Terrain*, ID3D11Device*);
	//	void Shutdown();
	//	void Render(XMFLOAT3*, Frustum*, ID3D11DeviceContext*, ShaderTerrain*);
	//
	//	int GetDrawCount();
	//
	//private:
	//	void CalculateMeshDimensions(int, float&, float&, float&);
	//	void CreateTreeNode(NodeType*, float, float, float, ID3D11Device*, int);
	//	int CountTriangles(float, float, float);
	//	bool IsTriangleContained(int, float, float, float);
	//	void ReleaseNode(NodeType*);
	//	void RenderNode(XMFLOAT3*, NodeType*, Frustum*, ID3D11DeviceContext*, ShaderTerrain*);
	//
	//private:
	//	int m_triangleCount, m_drawCount;
	//	VertexType* m_vertexList;
	//	NodeType* mRootNode;

public:
	QuadTree(int lvl, ID3D11Device*, PerlinNoise*, XMFLOAT3, int);

	bool RenderTree(ID3D11DeviceContext*, XMFLOAT3*);
	void GetVisibleTerrain(XMFLOAT3*, vector<Terrain*>*);

private:
	QuadTree* mChildNodes[4];
	Terrain* mTerrain;
	int mLevel;
	XMFLOAT3 mNodePosition;


};

#endif