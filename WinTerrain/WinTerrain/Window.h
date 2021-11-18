#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>

#include "App.h"

class Window
{
public:
	Window(App*, HINSTANCE,int,int);
	~Window() {};

	void Run();

private:
	static LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
	int StartWindow(int, int);

	App* app;
	HWND wnd;

	HINSTANCE hInstance;


	bool Frame();

};

