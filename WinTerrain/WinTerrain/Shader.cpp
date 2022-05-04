#include "Shader.h"

Shader::Shader(ID3D11Device* renderer)
{
    pRenderer = renderer;
}

Shader::~Shader()
{
}

void Shader::InitShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
    CreateVertexShader(vsFilename);
    CreatePixelShader(psFilename);

    CreateInputLayout();

    D3D11_BUFFER_DESC matrixBufferDesc;

    // Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
    matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
    matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    matrixBufferDesc.MiscFlags = 0;
    matrixBufferDesc.StructureByteStride = 0;

    D3D11_SAMPLER_DESC samplerDesc;

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
    pRenderer->CreateSamplerState(&samplerDesc, &sampleState);

	// Setup light buffer
	D3D11_BUFFER_DESC lightBufferDesc;

	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.ByteWidth = sizeof(LightBufferType);
	lightBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.MiscFlags = 0;
	lightBufferDesc.StructureByteStride = 0;
    pRenderer->CreateBuffer(&lightBufferDesc, NULL, &lightBuffer);

    // Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
    pRenderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);
}

bool Shader::CreateVertexShader(const wchar_t* vsFilename)
{
    HRESULT hr = D3DReadFileToBlob(vsFilename, &pVsBlob);
    assert(SUCCEEDED(hr));

    hr = pRenderer->CreateVertexShader(pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(), NULL, &pVertexShader);
    assert(SUCCEEDED(hr));

    return true;
}

bool Shader::CreatePixelShader(const wchar_t* psFilename)
{
    HRESULT hr = D3DReadFileToBlob(psFilename, &pPsBlob);
    assert(SUCCEEDED(hr));

    hr = pRenderer->CreatePixelShader(pPsBlob->GetBufferPointer(), pPsBlob->GetBufferSize(), NULL, &pPixelShader);
    assert(SUCCEEDED(hr));

    return true;
}

void Shader::CreateInputLayout()
{    
    D3D11_INPUT_ELEMENT_DESC inputElementDesc[] = {
          { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },          
          { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
          { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
          { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
          
    };
    
    HRESULT hr = pRenderer->CreateInputLayout(inputElementDesc, ARRAYSIZE(inputElementDesc), pVsBlob->GetBufferPointer(), pVsBlob->GetBufferSize(), &pInputLayout);
    assert(SUCCEEDED(hr));
}

void Shader::Render(ID3D11DeviceContext* deviceCtx, int indexCount)
{
    deviceCtx->IASetInputLayout(pInputLayout);

    /*** set vertex shader to use and pixel shader to use, and constant buffers for each ***/
    deviceCtx->VSSetShader(pVertexShader, NULL, 0);
    deviceCtx->PSSetShader(pPixelShader, NULL, 0);

    deviceCtx->DrawIndexed(indexCount, 0, 0);
}

void Shader::SetShaderParams(ID3D11DeviceContext* deviceContext, const DirectX::XMMATRIX& worldMatrix, const DirectX::XMMATRIX& viewMatrix, const DirectX::XMMATRIX& projectionMatrix, ID3D11ShaderResourceView* texture)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    MatrixBufferType* dataPtr;
    DirectX::XMMATRIX tworld, tview, tproj;

    // Transpose the matrices to prepare them for the shader.
    tworld = XMMatrixTranspose(worldMatrix);
    tview = XMMatrixTranspose(viewMatrix);
    tproj = XMMatrixTranspose(projectionMatrix);

    // Lock the constant buffer so it can be written to.
   deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
   dataPtr = (MatrixBufferType*)mappedResource.pData;
   dataPtr->world = tworld;// worldMatrix;
   dataPtr->view = tview;
   dataPtr->projection = tproj;
   deviceContext->Unmap(matrixBuffer, 0);

   // Send light data to pixel shader
   LightBufferType* lightPtr;
   deviceContext->Map(lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
   lightPtr = (LightBufferType*)mappedResource.pData;
   lightPtr->ambiant = DirectX::XMFLOAT4(0.4f, 0.4f, 0.4f, 0.4f);
   lightPtr->diffuse = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f); 
   lightPtr->direction = DirectX::XMFLOAT3(0.9f, -0.0f, 0.0f);
   lightPtr->padding = 0.0f;
   deviceContext->Unmap(lightBuffer, 0);
   deviceContext->PSSetConstantBuffers(0, 1, &lightBuffer);

    // Now set the constant buffer in the vertex shader with the updated values.
    deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

    deviceContext->PSSetShaderResources(0, 1, &texture);
    deviceContext->PSSetSamplers(0, 1, &sampleState);
}
