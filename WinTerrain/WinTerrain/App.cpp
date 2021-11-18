#include "App.h"

void App::Init(HWND hwnd)
{
    renderer = new Renderer(hwnd);
}

bool App::Frame()
{
    Render();

    return true;
}

bool App::Render()
{
    renderer->beginScene(0.3f,0.1f,0.8f,0);

    renderer->endScene();
    return false;
}
