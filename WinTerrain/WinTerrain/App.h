#pragma once
#include "Renderer.h"

class App
{
public:

	bool Frame();
	bool Render();

	void Init(HWND);

private:

	Renderer* renderer;
};

