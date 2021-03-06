#pragma once

#include <d3d11.h>
#include <D3Dcompiler.h>
#include <dxgi.h>
#include <DirectXMath.h>

class Shader
{
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 ambiant;
		DirectX::XMFLOAT4 diffuse;
		DirectX::XMFLOAT3 direction;
		float padding;
	};
public:

	Shader(ID3D11Device*);
	~Shader();

	void InitShader(const wchar_t* vsFilename, const wchar_t* psFilename);

	void Render(ID3D11DeviceContext* deviceCtx, int indexCount);
	void SetShaderParams(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix, ID3D11ShaderResourceView*);

private:
	ID3D11Device* pRenderer;
	ID3D11InputLayout* pInputLayout;

	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;

	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;

	ID3DBlob* pVsBlob;
	ID3DBlob* pPsBlob;

	ID3D11Buffer* lightBuffer;

	bool CreateVertexShader(const wchar_t* vsFilename);
	bool CreatePixelShader(const wchar_t* psFilename);
	void CreateInputLayout();
};

