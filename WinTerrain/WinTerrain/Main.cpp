#include "Window.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR pScmdline, int iCmdshow)
{
	App* app = new App();

	Window* wnd = new Window(app, hInstance, 1200, 675);

	wnd->Run();

	return 0;
}
