#include "App.h"

void App::Init(HWND hwnd, Input* in)
{
    renderer = new Renderer(hwnd);

    cube = new CubeMesh(renderer->getDevice(), renderer->getDeviceContext(), 1);

    shader = new Shader(renderer->getDevice());

    shader->InitShader(L"color_vs.cso",L"color_ps.cso");

    input = in;

    cam = new FPCamera(input, 1200, 675, hwnd);
    cam->setPosition(0.0f, 0.0f, -10.0f);
    cam->update();
}

bool App::Frame(float dt)
{
    cam->update();

    cam->move(dt);

    std::wstring blah;
    blah = std::to_wstring(cam->getPosition().x);

    OutputDebugString(blah.c_str());
    OutputDebugString(L"\n");

    Render();

    return true;
}

bool App::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

    worldMatrix = renderer->getWorldMatrix();
    viewMatrix = cam->getViewMatrix();
    projectionMatrix = renderer->getProjectionMatrix();

    cube->sendData(renderer->getDeviceContext());
    shader->SetShaderParams(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix);
    shader->Render(renderer->getDeviceContext(), cube->getIndexCount());

    renderer->endScene();
    return false;
}
