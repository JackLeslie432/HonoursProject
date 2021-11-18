#pragma once
#include "LSystem.h"

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

class App
{
public:

	App();
	~App();

	void run();

	bool Frame();
	bool Render();

private:
	LSystem system;

	HWND wnd;
	int sWidth, sHeight;

};
