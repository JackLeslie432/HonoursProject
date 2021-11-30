#pragma once

#include <d3d11.h>
#include "DirectXTK/WICTextureLoader.h"
#include <assert.h>
#include <string>
#include <fstream>
#include <map>


class TextureManager
{
public:

	TextureManager(ID3D11Device*, ID3D11DeviceContext*);
	~TextureManager();

	ID3D11ShaderResourceView* GetTexture(const wchar_t* uid);
	void LoadTexture(const wchar_t* uid ,const wchar_t* filename);

protected:

	void GenerateTexture();

	ID3D11ShaderResourceView* texture;
	ID3D11Texture2D* pTexture;

	std::map<wchar_t*, ID3D11ShaderResourceView*> textureMap;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceCtx;
};

