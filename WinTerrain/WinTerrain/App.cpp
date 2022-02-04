#include "App.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"

void App::Init(HWND hwnd, Input* in)
{
    renderer = new Renderer(hwnd);

    textureMgr = new TextureManager(renderer->getDevice(),renderer->getDeviceContext());
    terrain = new TerrainPlane(renderer->getDevice());

    shader = new Shader(renderer->getDevice());
    shader->InitShader(L"color_vs.cso",L"color_ps.cso");

	ImGui::CreateContext();

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(renderer->getDevice(), renderer->getDeviceContext());

    input = in;

    textureMgr->LoadTexture(L"grass", L"res/grass.png");

    cam = new FPCamera(input, 1200, 675, hwnd);
    cam->setPosition(0.0f, 0.0f, -10.0f);
    cam->update();

    InitLSystem();
}

bool App::Frame(float dt)
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
    ImGui::Begin("Cum");

    cam->update();
    cam->move(dt);

    ImGui::End();

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

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

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

	temp.first = "ab";
	temp.second = 50;

	rules.push_back(temp);

	system.AddStochRule('a', rules);

	// Adding context rules
	system.AddContextRule('a', 'a', 'b', "c");

	system.Run(3);    

    return true;
}