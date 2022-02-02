#include "App.h"

void App::Init(HWND hwnd, Input* in)
{

    renderer = new Renderer(hwnd);

    textureMgr = new TextureManager(renderer->getDevice(),renderer->getDeviceContext());
    terrain = new TerrainPlane(renderer->getDevice());

    shader = new Shader(renderer->getDevice());
    shader->InitShader(L"color_vs.cso",L"color_ps.cso");

    input = in;

    textureMgr->LoadTexture(L"grass", L"res/grass.png");

    cam = new FPCamera(input, 1200, 675, hwnd);
    cam->setPosition(0.0f, 0.0f, -10.0f);
    cam->update();

    InitLSystem();
}

bool App::Frame(float dt)
{
    cam->update();

    cam->move(dt);

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

    terrain->SendData(renderer->getDeviceContext());
    shader->SetShaderParams(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->GetTexture(L"grass"));
    shader->Render(renderer->getDeviceContext(), terrain->GetIndexCount());

    renderer->endScene();
    return false;
}

bool App::InitLSystem()
{
	LSystem system("a");
	system.SetUsedRules(true, true);

	// Adding normal rules
	system.AddRule('a', "c");

	// Adding stochastic rules
	std::vector<std::pair<string, float>> rules;
	std::pair<string, float> temp;

	temp.first = "a";
	temp.second = 50;

	rules.push_back(temp);

	temp.first = "b";
	temp.second = 50;

	rules.push_back(temp);

	system.AddStochRule('a', rules);

	// Adding context rules
	system.AddContextRule('a', 'a', 'b', "c");

	system.Iterate();    

    return true;
}