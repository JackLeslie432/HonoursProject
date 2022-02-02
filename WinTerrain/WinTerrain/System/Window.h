#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "/Users/jjac0/Documents/GitHub/HonoursProject/WinTerrain/WinTerrain/App.h"
#include "Input.h"
#include "Timer.h"

class Window
{
public:
	Window(App*, HINSTANCE,int,int);
	~Window();

	void Run();

private:
	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
	int StartWindow(int, int);

	App* app;
	HWND wnd;

	HINSTANCE hInstance;
	Input input;	

	core::Timer timer;
};

static Window* _Application_Handle = nullptr;