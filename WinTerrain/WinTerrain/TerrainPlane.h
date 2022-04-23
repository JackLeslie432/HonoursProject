#pragma once

#include <d3d11.h>
#include <directxmath.h>
#include <vector>

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

	public void CreateIsland();

	int Resolution() { return resolution; };
	float* GetHeightMap() { return heightMap; };

	struct PerlinSettings
	{
		float scale = 8;
		float frequency = 0.1;
		float offset = 0.5;
		int octaves = 5;
	};

	struct IslandSettings
	{
		XMFLOAT2 centre;
		XMFLOAT2 size = XMFLOAT2(100,100);

		float height = 50;
		int amount = 0;
	};

	void SetPerlin(PerlinSettings val) {  perlinSettings = val;};
	void SetIsland(IslandSettings val) {  islandSettings = val;};

	std::vector<XMFLOAT2> GetIslandCenter() { return islandCentres; };
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

	// Particle
	void ParticleDeposition(int particlePointX, int particlePointZ);

	// Plane variables
	ID3D11Buffer* vertexBuffer, * indexBuffer;
	int vertexCount, indexCount, resolution;
	float scale;

	// Settings variables
	PerlinSettings perlinSettings;
	IslandSettings islandSettings;

	std::vector<XMFLOAT2> islandCentres;

	// Height
	float* heightMap;
};

