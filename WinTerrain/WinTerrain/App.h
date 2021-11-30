#pragma once
#include "Renderer.h"
#include "TerrainPlane.h"
#include "Shader.h"
#include "TextureManager.h"
#include "FPCamera.h"

#include <string>

class App
{
public:

	bool Frame(float);
	bool Render();

	void Init(HWND, Input*);

private:

	Renderer* renderer;
	TextureManager* textureMgr;

	TerrainPlane* terrain;

	Shader* shader;
	FPCamera *cam;

	Input* input;
	
};

