#pragma once

#include <d3d11.h>
#include <directxmath.h>

class TerrainPlane
{
protected:

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 texture;
	};

public:

	TerrainPlane(ID3D11Device*);
	~TerrainPlane();

	void SendData(ID3D11DeviceContext*, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	int GetIndexCount() { return indexCount; };

	void Resize(int);

protected:

	void CreateBuffers(ID3D11Device* ,VertexType* ,unsigned long* );

	VertexType* BuildMesh();
	unsigned long* BuildIndex();

	void AssignHeight();

	ID3D11Buffer* vertexBuffer, * indexBuffer;
	int vertexCount, indexCount, resolution;
	float scale;

	float* heightMap;
};

