#pragma once

#include "System/Renderer.h"

#include "TerrainPlane.h"
#include "TextureManager.h"
#include "Shader.h"
#include "FPCamera.h"
#include "LSystem.h"
#include "Trees.h"

#include <string>

#include "imgui_internal.h"

class App
{
public:

	bool Frame(float);
	bool Render();

	void Init(HWND, Input*);
private:

	Input* input;

	Renderer* renderer;
	TextureManager* textureMgr;
	Shader* shader;
	FPCamera *cam;

	TerrainPlane::IslandSettings islandSettings;

	// Terrain settings
	TerrainPlane* terrain;

	// variables for terrain control
	int islandAmount = 0;
	int treeAmount = 0;
	int mountainAmount = 0;
	int runAmount = 0;
	bool reset = true;

	bool isSettingsOpen = false;

	LSystem* system;
	
	// Local functions
	bool InitLSystem();
	bool RegenSystem();
	void DrawGUI();

	Trees* tree;

	enum OpenSetings
	{
		Base,
		Island,
		Tree,
	};

	OpenSetings openSettings = Base;

};

