#pragma once

#include <d3d11.h>
#include <directxmath.h>

using namespace DirectX;

class TerrainPlane
{
protected:

	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT3 colour;
		XMFLOAT2 texture;
		XMFLOAT3 normal;
	};

public:

	TerrainPlane(ID3D11Device*);
	~TerrainPlane();

	void SendData(ID3D11DeviceContext*, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	int GetIndexCount() { return indexCount; };

	void Resize(int);
	void Regenerate(float* heightMap, ID3D11Device* device);

	int Resolution() { return resolution; };
	float* GetHeightMap() { return heightMap; };

	float& PerlinScale() { return perlinScale; };
	float& PerlinFrequency() { return perlinFrequency; };
	float& PerlinOffset() { return perlinOffset; };
	int& PerlinOctaves() { return perlinOctaves; };

	//float& IslandCenterX() { return islandCentre.x; };
	//float& IslandCentreY() { return islandCentre.y; };
	XMFLOAT2& IslandCentre() { return islandCentre; };
	float& IslandSizeX() { return islandSizeX; };
	float& IslandSizeY() { return islandSizeY; };
	float& IslandHeight() { return islandHeight; };
protected:

	void CreateBuffers(ID3D11Device* ,VertexType* ,unsigned long* );

	VertexType* BuildMesh();
	unsigned long* BuildIndex();

	void AssignHeight(float* height);

	// Perlin functions
	void PerlinNoise(float scale, float offset, float frequency, int octaves);
	float PerlinNoiseCalc(float x, float y);
	float Gradient(int hash, float x, float y);

	// Quadratic distance
	void QuadraticDistance(float sizeX, float sizeY, float centerX, float centerY, float height);

	// Plane variables
	ID3D11Buffer* vertexBuffer, * indexBuffer;
	int vertexCount, indexCount, resolution;
	float scale;

	// Perlin variables
	float perlinScale = 20;
	float perlinFrequency = 0.1;
	float perlinOffset = 0.5;
	int perlinOctaves = 5;

	// Island variables
	XMFLOAT2 islandCentre;

	float islandSizeX = 50;
	float islandSizeY = 50;

	float islandHeight = 50;

	// Height
	float* heightMap;
};

