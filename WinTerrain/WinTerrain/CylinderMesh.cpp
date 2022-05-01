#include "CylinderMesh.h"

// Initialise buffers and lad texture.
CylinderMesh::CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, int inResolution = 5)
{
	resolution = inResolution;
	initBuffers(device);
}

// Release resources.
CylinderMesh::~CylinderMesh()
{
	// Run parent deconstructor
	BaseMesh::~BaseMesh();
}

void CylinderMesh::initBuffers(ID3D11Device* device)
{
	int segments = resolution;
	int height = 10;

	// Vertex Variables
	// Latitude  variable
	float lat = 0;

	// Variables for incrementing the latitude and longitude
	float nextLatitude = (2 * 3.1415) / segments;

	// Texture co-ord variables
		// Vaiables for storing texture positions
	float textPlaceLat = 0;
	float textPlaceHeight = 0;

	// Variable for incrementing texture co-ords
	float textIncrementLat = 1 / segments * 2;
	float textIncrementHeight = 1 / height;

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	vertexCount = (segments *4) * height*2;
	indexCount = (segments * 4) * height*2;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	int v = 0;
	int in = 0;

	for (int i = 0; i < height; i++) // loop for the longitude
	{
		for (int j = 0; j < segments; j++) // Loop for latitude
		{
			// Tri 1
			// [lat +1][height]
			vertices[v].normal = XMFLOAT3(cos(lat + nextLatitude), 0, sin(lat + nextLatitude));
			vertices[v].texture = XMFLOAT2(textPlaceLat + textIncrementLat, textPlaceHeight);
			vertices[v].position = XMFLOAT3(cos(lat + nextLatitude), i, sin(lat + nextLatitude));

			indices[in] = in;
			in++;
			v++;	
			// [lat +1][height+1]
			vertices[v].normal = XMFLOAT3(cos(lat + nextLatitude), 0, sin(lat + nextLatitude));
			vertices[v].texture = XMFLOAT2(textPlaceLat + textIncrementLat, textPlaceHeight + textIncrementHeight);
			vertices[v].position = XMFLOAT3(cos(lat + nextLatitude), i + 1, sin(lat + nextLatitude));

			indices[in] = in;
			in++;
			v++;
			// [lat][height]
			vertices[v].normal = XMFLOAT3(cos(lat), 0, sin(lat));
			vertices[v].texture = XMFLOAT2(textPlaceLat, textPlaceHeight);
			vertices[v].position = XMFLOAT3(cos(lat), i, sin(lat));	

			indices[in] = in;
			in++;
			v++;
		
			// Tri 2
			// [lat +1][height+1]
			vertices[v].normal = XMFLOAT3(cos(lat + nextLatitude), 0, sin(lat + nextLatitude));
			vertices[v].texture = XMFLOAT2(textPlaceLat + textIncrementLat, textPlaceHeight + textIncrementHeight);
			vertices[v].position = XMFLOAT3(cos(lat + nextLatitude), i + 1, sin(lat + nextLatitude));

			indices[in] = in;
			in++;
			v++;	// [lat][height+1]
			vertices[v].normal = XMFLOAT3(cos(lat), 0, sin(lat));
			vertices[v].texture = XMFLOAT2(textPlaceLat, textPlaceHeight + textIncrementHeight);
			vertices[v].position = XMFLOAT3(cos(lat), i + 1, sin(lat));	

			indices[in] = in;
			in++;
			v++;
			// [lat][height]
			vertices[v].normal = XMFLOAT3(cos(lat), 0, sin(lat));
			vertices[v].texture = XMFLOAT2(textPlaceLat, textPlaceHeight);
			vertices[v].position = XMFLOAT3(cos(lat), i, sin(lat));

			indices[in] = in;
			in++;
			v++;

			// Increment angle of latitude
			lat += nextLatitude;

			// Increment texture co-ord for latitude
			textPlaceLat += textIncrementLat;
		}

		// Texture coords
			// Incrementing the Height texture co-ord
		textPlaceHeight += textIncrementHeight;

		// Reset the latitude co-ord
		textPlaceLat = 0;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
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

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;
	delete[] indices;
	indices = 0;
}