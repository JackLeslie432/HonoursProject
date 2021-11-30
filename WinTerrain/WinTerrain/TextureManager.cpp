#include "TextureManager.h"

TextureManager::TextureManager(ID3D11Device* inDevice, ID3D11DeviceContext* inDeviceCtx)
{
    device = inDevice;
    deviceCtx = inDeviceCtx;

}

TextureManager::~TextureManager()
{

}

void TextureManager::LoadTexture(const wchar_t* uid,const wchar_t* filename)
{
   
    std::ifstream infile(filename);
    if (infile.good())
    {

        HRESULT hr = DirectX::CreateWICTextureFromFile(device, deviceCtx, filename, NULL, &texture);
        assert(SUCCEEDED(hr));

        textureMap.insert(std::make_pair(const_cast<wchar_t*>(uid), texture));
    }
}

void TextureManager::GenerateTexture()
{
    // Create texture
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = 256;
    desc.Height = 256;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.MiscFlags = 0;

    HRESULT hr = device->CreateTexture2D(&desc, NULL, &pTexture);
}

ID3D11ShaderResourceView* TextureManager::GetTexture(const wchar_t* uid)
{
    ID3D11ShaderResourceView* tex = nullptr;

    if (textureMap.size() > 0)
        tex = textureMap.at(const_cast<wchar_t*>(uid));

    if (tex)    
        return tex;
    else
	    return nullptr;
}