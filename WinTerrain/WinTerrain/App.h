#pragma once

#include "System/Renderer.h"

#include "TerrainPlane.h"
#include "TextureManager.h"
#include "Shader.h"
#include "FPCamera.h"
#include "LSystem.h"

#include <string>

#include "imgui_internal.h"

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
	
	bool InitLSystem();
};

