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
	HWND wnd;


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

	// L-system Generation
	bool InitLSystem();
	bool RegenSystem();

	// Terrain Generation
	void GenerateTerrain();

	// Draw GUI
	void DrawGUI();
	void DrawLSystemGUI();
	void DrawTerrainGUI();

	void AddTree(XMFLOAT2 position);

	std::vector<Trees*> trees;

	struct TreeSettings
	{
		float density = 10;
		XMFLOAT2 size;
	};

	TreeSettings treeSettings;

	enum OpenSetings
	{
		Base,
		Island,
		Tree,
		Camera
	};

	OpenSetings openSettings = Base;
	TerrainPlane::IslandSettings islandSettings;

	core::Timer time;
};

