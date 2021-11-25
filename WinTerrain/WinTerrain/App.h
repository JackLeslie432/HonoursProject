#pragma once
#include "Renderer.h"
#include "CubeMesh.h"
#include "Shader.h"
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

	CubeMesh* cube;

	Shader* shader;
	FPCamera *cam;

	Input* input;
	
};

