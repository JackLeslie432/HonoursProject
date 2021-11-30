#include "TerrainPlane.h"

TerrainPlane::TerrainPlane(ID3D11Device* device)
{
	vertexBuffer = nullptr;
	indexBuffer = nullptr;
	vertexCount = 0;
	indexCount = 0;
	scale = 1;

	resolution = 100;

	Resize(resolution);
	AssignHeight();

	vertexCount = resolution * resolution;
	indexCount = ((resolution - 1) * (resolution - 1)) * 6;

	CreateBuffers(device, BuildMesh(), BuildIndex());
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
	increment = 10 / resolution;

	//Set up vertices
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			positionX = (float)j * scale;
			positionZ = (float)(i)*scale;

			height = heightMap[index];
			vertices[index].position = DirectX::XMFLOAT3(positionX, height, positionZ);
			vertices[index].texture = DirectX::XMFLOAT2(u, v);

			u += increment;
			index++;
		}
		u = 0;
		v += increment;
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

void TerrainPlane::SendData(ID3D11DeviceContext* deviceContext, D3D_PRIMITIVE_TOPOLOGY top)
{
	unsigned int stride;
	unsigned int offset;

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->IASetPrimitiveTopology(top);
}

void TerrainPlane::Resize(int inResolution) 
{
	resolution = inResolution;
	heightMap = new float[resolution * resolution];
	if (vertexBuffer != NULL) {
		vertexBuffer->Release();
	}
	vertexBuffer = NULL;
}

void TerrainPlane::AssignHeight()
{
	// Gives a random number from 1 to 20 for each place in the height map
	for (int j = 0; j < (resolution); j++) {
		for (int i = 0; i < (resolution); i++) {
			heightMap[(j * resolution) + i] = rand() % 20 + 1;
			//heightMap[(j * resolution) + i] = 0;
		}
	}
}
