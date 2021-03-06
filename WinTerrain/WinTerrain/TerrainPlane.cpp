#include "TerrainPlane.h"
#include <cstdlib>
#include <cmath>
#include <ctime>

// Interpolation
#define FADE(t) ( t * t * t * ( t * ( t * 6 - 15 ) + 10 ) )
#define LERP(t, a, b) (a + t*(b-a))

// Permutations for Perlin noise
unsigned int perm[] = { 151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180,
  151,160,137,91,90,15,
  131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
  190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
  88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
  77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
  102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
  135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
  5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
  223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
  129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
  251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
  49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
  138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
};

TerrainPlane::TerrainPlane(ID3D11Device* device)
{
	// init buffers
	vertexBuffer = nullptr;
	indexBuffer = nullptr;

	vertexCount = 0;
	indexCount = 0;

	// Stating size of terrain
	scale = 1;
	resolution = 500;

	// Set set and generate heights for terrain
	Resize(resolution);

	// Assign correct vertex and index count
	vertexCount = resolution * resolution;
	indexCount = ((resolution - 1) * (resolution - 1)) * 6;
		
	CreateBuffers(device, BuildMesh(), BuildIndex());

	srand(time(NULL));
}

TerrainPlane::~TerrainPlane()
{
	if (vertexBuffer)
	{
		vertexBuffer->Release(); 
		vertexBuffer = NULL;
	}
	
	if (indexBuffer)
	{
		indexBuffer->Release();
		indexBuffer = NULL;
	}
}

void TerrainPlane::CreateBuffers(ID3D11Device* device, VertexType* vertices, unsigned long* indices)
{
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Set up the description of the dyanmic vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);
}

TerrainPlane::VertexType* TerrainPlane::BuildMesh()
{
	int index = 0;
	float positionX, height, positionZ, u, v, increment;

	VertexType* vertices = new VertexType[vertexCount];

	u = 0;
	v = 0;
	increment = 100.f / resolution;
	
	//Set up vertices
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			positionX = float(j) * increment * scale;
			positionZ = float(i) * increment * scale;

			//positionX = float(j)* scale;
			//positionZ = float(i)* scale;

			height = heightMap[index];
			vertices[index].position = DirectX::XMFLOAT3(positionX, height, positionZ);
			vertices[index].texture = DirectX::XMFLOAT2(u, v);

			u += increment;
			index++;
		}
		u = 0;
		v += increment;
	}

	for (int j = 0; j < (resolution - 1); j++) {
		for (int i = 0; i < (resolution - 1); i++) {
			//Calculate the plane normals
			DirectX::XMFLOAT3 a, b, c;	//Three corner vertices
			a = vertices[j * resolution + i].position;
			b = vertices[j * resolution + i + 1].position;
			c = vertices[(j + 1) * resolution + i].position;

			//Two edges
			DirectX::XMFLOAT3 ab(c.x - a.x, c.y - a.y, c.z - a.z);
			DirectX::XMFLOAT3 ac(b.x - a.x, b.y - a.y, b.z - a.z);

			//Calculate the cross product
			DirectX::XMFLOAT3 cross;
			cross.x = ab.y * ac.z - ab.z * ac.y;
			cross.y = ab.z * ac.x - ab.x * ac.z;
			cross.z = ab.x * ac.y - ab.y * ac.x;
			float mag = (cross.x * cross.x) + (cross.y * cross.y) + (cross.z * cross.z);
			mag = sqrtf(mag);
			cross.x /= mag;
			cross.y /= mag;
			cross.z /= mag;
			vertices[j * resolution + i].normal = cross;
		}
	}

	//Smooth the normals by averaging the normals from the surrounding planes
	DirectX::XMFLOAT3 smoothedNormal(0, 1, 0);
	for (int j = 0; j < resolution; j++) {
		for (int i = 0; i < resolution; i++) {
			smoothedNormal.x = 0;
			smoothedNormal.y = 0;
			smoothedNormal.z = 0;
			float count = 0;
			//Left planes
			if ((i - 1) >= 0) {
				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal.x += vertices[j * resolution + (i - 1)].normal.x;
					smoothedNormal.y += vertices[j * resolution + (i - 1)].normal.y;
					smoothedNormal.z += vertices[j * resolution + (i - 1)].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal.x += vertices[(j - 1) * resolution + (i - 1)].normal.x;
					smoothedNormal.y += vertices[(j - 1) * resolution + (i - 1)].normal.y;
					smoothedNormal.z += vertices[(j - 1) * resolution + (i - 1)].normal.z;
					count++;
				}
			}
			//right planes
			if ((i) < (resolution - 1)) {

				//Top planes
				if ((j) < (resolution - 1)) {
					smoothedNormal.x += vertices[j * resolution + i].normal.x;
					smoothedNormal.y += vertices[j * resolution + i].normal.y;
					smoothedNormal.z += vertices[j * resolution + i].normal.z;
					count++;
				}
				//Bottom planes
				if ((j - 1) >= 0) {
					smoothedNormal.x += vertices[(j - 1) * resolution + i].normal.x;
					smoothedNormal.y += vertices[(j - 1) * resolution + i].normal.y;
					smoothedNormal.z += vertices[(j - 1) * resolution + i].normal.z;
					count++;
				}
			}
			smoothedNormal.x /= count;
			smoothedNormal.y /= count;
			smoothedNormal.z /= count;

			float mag = sqrt((smoothedNormal.x * smoothedNormal.x) + (smoothedNormal.y * smoothedNormal.y) + (smoothedNormal.z * smoothedNormal.z));
			smoothedNormal.x /= mag;
			smoothedNormal.y /= mag;
			smoothedNormal.z /= mag;

			vertices[j * resolution + i].normal = smoothedNormal;
		}
	}
	return vertices;
}

unsigned long* TerrainPlane::BuildIndex()
{
	unsigned long* indices = new unsigned long[indexCount];

	//Set up index list
	int index = 0;
	for (int j = 0; j < (resolution - 1); j++) {
		for (int i = 0; i < (resolution - 1); i++) {

			//Build index array
			indices[index] = (j * resolution) + i;
			indices[index + 1] = ((j + 1) * resolution) + (i + 1);
			indices[index + 2] = ((j + 1) * resolution) + i;

			indices[index + 3] = (j * resolution) + i;
			indices[index + 4] = (j * resolution) + (i + 1);
			indices[index + 5] = ((j + 1) * resolution) + (i + 1);
			index += 6;
		}
	}

	return indices;
}

void TerrainPlane::Regenerate(float* heightMap_)
{		
	// Alter height map with Perlin noise
	PerlinNoise(perlinSettings.scale/perlinSettings.octaves, perlinSettings.octaves, perlinSettings.frequency, perlinSettings.octaves);
}

void TerrainPlane::CreateIsland(XMFLOAT2 islandPosition)
{
	int tempSizeX,tempSizeY,tempHeight;

	// Add a random size and height to the islands
	tempSizeX = islandSettings.size.x + rand() % (int)islandSettings.size.x / 2 - (int)islandSettings.size.x / 4;
	tempSizeY = islandSettings.size.y + rand() % (int)islandSettings.size.y/2 - (int)islandSettings.size.y/4;
	tempHeight = islandSettings.height + rand() % (int)islandSettings.height / 2 - (int)islandSettings.height / 4;

	// Create island with given numbers
	QuadraticDistance(tempSizeY, tempSizeY, islandPosition.x, islandPosition.y, tempHeight);
}

void TerrainPlane::BuildMap(ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{	
	m_vertices = BuildMesh();
	m_indices = BuildIndex();

	if (vertexBuffer == nullptr)			
		CreateBuffers(device, m_vertices, m_indices);	// Build the map to be rendered
	else // Update information if buffers created
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

		// Disable the GPU access and update
		deviceContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);		
		memcpy(mappedResource.pData, m_vertices, sizeof(VertexType) * vertexCount);
		deviceContext->Unmap(vertexBuffer, 0);
	}
	
	delete[] m_vertices;
	m_vertices = NULL;
	delete[] m_indices;
	m_indices = NULL;
}

void TerrainPlane::SendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Send the vertex's to the graphics
	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}

void TerrainPlane::Resize(int inResolution) 
{
	// Change the amount of vertexes in terrain
	resolution = inResolution;
	heightMap = new float[resolution * resolution];

	// Assign correct vertex and index count
	vertexCount = resolution * resolution;
	indexCount = ((resolution - 1) * (resolution - 1)) * 6;

	if (vertexBuffer != NULL) 
		vertexBuffer->Release();
	vertexBuffer = NULL;

	if (indexBuffer != NULL) 
		indexBuffer->Release();	
	indexBuffer = NULL;
}

void TerrainPlane::PerlinNoise(float scale, float offset, float frequency, int octaves)
{
	// Loop for each vertex of the plane
	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{			
			// X & Y Co-ords
			float x = (j + offset) * frequency;
			float y = (i + offset) * frequency;

			// Find the initial height
			float height = scale * PerlinNoiseCalc(x, y);

			// Give more detail with octaves
			for (int n = 2; n < octaves + 2; n++)
			{
				float lacunarity = std::pow(2, n);

				x = (j + offset) * lacunarity;
				y = (i + offset) * lacunarity;

				float persistance = std::pow(0.5, n);

				height += (persistance) * PerlinNoiseCalc(x, y);
			}
			
			heightMap[(i * resolution + j)] = height;
		}
	}
}

float TerrainPlane::PerlinNoiseCalc(float x, float y)
{
	int ix = (int)floor(x) & 255;		// Integer part of x
	int iy = (int)floor(y) & 255;		// Integer part of y
	x -= floor(x);						// Fractional part of x
	y -= floor(y);						// Fractional part of y

	float u = FADE(x); //6t^5-15t^4+10t^3
	float v = FADE(y); //6t^5-15t^4+10t^3

	// Hash the co-ords of the corners
	int A = perm[ix] + iy, AA = perm[A], AB = perm[A + 1],
		B = perm[ix + 1] + iy, BA = perm[B], BB = perm[B + 1];

	// LERP between all corners
	return	LERP(v,
		LERP(u, Gradient(perm[AA], x, y),
			Gradient(perm[BA], x - 1, y)),
		LERP(u, Gradient(perm[AB], x, y - 1),
			Gradient(perm[BB], x - 1, y - 1)));
}

float TerrainPlane::Gradient(int hash, float x, float y)
{
	int h = hash & 7;			// Getting 3 bits worth of the hash

	float u = h < 4 ? x : y;	// if hash < 4 u = x otherwise u = y
	float v = h < 4 ? y : x;	// if hash < 4 v = y otherwise v = x

	return ((h & 1) ? -u : u) + ((h & 2) ? v : -v);
}

void TerrainPlane::QuadraticDistance(float sizeX, float sizeY, float centerX, float centerY, float height)
{
	// size of the mask 
	float max_width = sizeX * 0.5f;
	float max_height = sizeY * 0.5f;

	for (int j = 0; j < resolution; j++)
	{
		for (int i = 0; i < resolution; i++)
		{
			// Distance from the centre of the mask
			float distance_x = fabs(i - centerX);
			float distance_y = fabs(j - centerY);

			float distance = sqrt(distance_x * distance_x + distance_y * distance_y); // distance to the circular mask			

			float deltax = distance / max_width;
			float deltay = distance / max_height;

			float gradient = deltax * deltay;

			// cutoff for when to stop gradient comparison
			if (gradient < 1)
				heightMap[(i * resolution + j)] += height;
			else
				heightMap[(i * resolution + j)] += height*(1/gradient);
		}
	}
}

void TerrainPlane::ParticleDeposition(int particlePointX, int particlePointZ)
{
	std::vector<XMFLOAT2> highList;		// List to store all the point needed to be checked
	std::vector<XMFLOAT2> lowestList;	// List to store the points that are at the lowest
	highList.push_back(XMFLOAT2(particlePointX, particlePointZ)); // Push back the starting point of the particle

	while (highList.size() != 0) // loop until all points have been checked
	{
		XMFLOAT2 tempPos = highList[rand() % highList.size()]; // Choose a random direction to go
		bool lowest = true; // Set bool to know if a point is lower than current (default to true)
		highList.clear(); // Remove the current points from the list

		float currentHeight = heightMap[int(tempPos.x * resolution + tempPos.y)];

		for (int i = -1; i < 2; i++)
		{
			for (int j = -1; j < 2;j++)
			{
				// In bounds check
				if ((tempPos.x + i) < 0 || (tempPos.x + i) > resolution || (tempPos.y + j) < 0 || (tempPos.y + j) > resolution)
					continue;

				if (currentHeight > heightMap[int((tempPos.x + i) * resolution + (tempPos.y + j))])
				{
					highList.push_back(XMFLOAT2(tempPos.x + i, tempPos.y + j)); // Add new point to the check list
					lowest = false; // Set lowest bool to false as this is lower than current point
				}
			}
		}

		if (lowest == true) // If the current point is the lowest add to list of the lowest points
		{
			heightMap[int(tempPos.x * resolution + tempPos.y)]++;
			return;
		}
	}
}
