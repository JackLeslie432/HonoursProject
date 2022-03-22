#pragma once

#include <d3d11.h>
#include <directxmath.h>

class TerrainPlane
{
protected:

	struct VertexType
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 colour;
		DirectX::XMFLOAT2 texture;
		DirectX::XMFLOAT3 normal;
	};

public:

	TerrainPlane(ID3D11Device*);
	~TerrainPlane();

	void SendData(ID3D11DeviceContext*, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	int GetIndexCount() { return indexCount; };

	void Resize(int);
	void Regenerate(float* heightMap, ID3D11Device* device);

	int Resolution() { return resolution; };

	float& PerlinScale() { return perlinScale; };
	float& PerlinFrequency() { return perlinFrequency; };
	float& PerlinOffset() { return perlinOffset; };
	int& PerlinOctaves() { return perlinOctaves; };

protected:

	void CreateBuffers(ID3D11Device* ,VertexType* ,unsigned long* );

	VertexType* BuildMesh();
	unsigned long* BuildIndex();

	void AssignHeight(float* height);

	void PerlinNoise(float scale, float offset, float frequency, int octaves);
	float PerlinNoiseCalc(float x, float y);
	float Gradient(int hash, float x, float y);

	ID3D11Buffer* vertexBuffer, * indexBuffer;
	int vertexCount, indexCount, resolution;
	float scale;

	float perlinScale = 20;
	float perlinFrequency = 0.1;
	float perlinOffset = 0.5;
	int perlinOctaves = 5;

	float* heightMap;
};

