#include "App.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "StochasticRule.h"
#include "Trees.h"
#include <ctime>
#include "System/Timer.h"

void App::Init(HWND hwnd, Input* in)
{
    // Allocate new managers
    renderer = new Renderer(hwnd);
    textureMgr = new TextureManager(renderer->getDevice(),renderer->getDeviceContext());
    terrain = new TerrainPlane(renderer->getDevice());

    shader = new Shader(renderer->getDevice());
    shader->InitShader(L"color_vs.cso",L"color_ps.cso");

    // Create and setup ImGui context
	ImGui::CreateContext();
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(renderer->getDevice(), renderer->getDeviceContext());

    input = in;
    wnd = hwnd;

    // Load in textures to be used
    textureMgr->LoadTexture(L"grass", L"res/grass.png");
    textureMgr->LoadTexture(L"bark", L"res/bark.png");
    textureMgr->LoadTexture(L"leaves", L"res/leaves.png");

    // Setup a new camera	
    cam = new FPCamera(input, 1200, 675, hwnd);
    cam->setPosition(0.0f, 0.0f, -10.0f);
	cam->setMoveSpeed(50,50);
    cam->update();

	treeSettings.size.x = 20;
	treeSettings.size.y = 20;

    InitLSystem();
}

bool App::Frame(float dt)
{
	if (input->isKeyDown(27))	
		return false;
	

    // Initialize ImGui window
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

    // Start a new tab for terrain

    DrawGUI();
    
    // Update the camera
    cam->update();
    cam->move(dt);

	time.frame();

    Render();

    return true;
}

bool App::Render()
{
    XMMATRIX worldMatrix, viewMatrix, projectionMatrix;

    // Set the background of the scene
    renderer->beginScene(0.39f, 0.58f, 0.92f, 1.0f);

    // Grab the required matrices for rendering
    worldMatrix = renderer->getWorldMatrix();
    viewMatrix = cam->getViewMatrix();
    projectionMatrix = renderer->getProjectionMatrix();

    // Run shader to render the terrain
    terrain->SendData(renderer->getDeviceContext());
    shader->SetShaderParams(renderer->getDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, textureMgr->GetTexture(L"grass"));
    shader->Render(renderer->getDeviceContext(), terrain->GetIndexCount());

    for (auto tree : trees)
	    tree->render(worldMatrix, viewMatrix, projectionMatrix, textureMgr->GetTexture(L"bark"), textureMgr->GetTexture(L"leaves"));

    // Render the ImGui window
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    
    renderer->endScene();
    return false;
}

bool App::InitLSystem()
{
    // Create a new L-system
	system = new LSystem("O");
	system->SetUsedRules(true, true);

    std::string islands;

	// Adding normal rules
    for (int i = 0; i < islandAmount; i++)    
        islands.append("[I]");
    
	system->AddRule('O', islands);

    // Add stochastic rules
    StochasticRule stochRules;

    stochRules.Add("T", 50);
    stochRules.Add("M", 50);

	system->AddStochRule('I', stochRules.GetRules());

	// Adding context rules
	system->AddContextRule('a', 'a', 'b', "c");

	system->Run(3, true);

    return true;
}

bool App::RegenSystem()
{
    if (system == nullptr)
        return false;

    // Clear all current rules
    system->ClearContextRule();
    system->ClearStochRule();
    system->ClearRule();

	system->SetUsedRules(true, false);

    // Reset system axiom
    system->Run(0, true);

    std::string islands;
	// Adding normal rules
	for (int i = 0; i < islandAmount; i++)
		islands.append("[I]");

	system->AddRule('O', islands);

	// Add stochastic rules
	StochasticRule stochRules;

	stochRules.Add("IT", 50);
	//stochRules.Add("IM", 50);

	system->AddStochRule('I', stochRules.GetRules());

    return true;
}

void App::GenerateTerrain()
{	
	time.getTime();
	
	RegenSystem();
	system->Run(2, true);

	std::vector<XMFLOAT2> PositionList;
	XMFLOAT2 CurrentPos(0, 0);

	for (auto tree : trees)
		tree->Destroy();

	trees.clear();

	terrain->Regenerate(nullptr);

	for (auto c : system->GetCurrentSystem())
	{
		switch (c)
		{
		case 'I':
			terrain->CreateIsland(CurrentPos);
			break;
		case '[':
			PositionList.emplace_back(XMFLOAT2(rand() % terrain->Resolution(), rand() % terrain->Resolution()));
			CurrentPos = PositionList.back();
			break;
		case ']':
			// Remove current Position
			PositionList.pop_back();
			if (!PositionList.empty())
				CurrentPos = PositionList.back();
			else
				CurrentPos = XMFLOAT2(0, 0);
			break;
		case 'T':
			AddTree(CurrentPos);
			break;
		case 'M':
			break;
		default:
			break;
		}
	}

	for (auto forest : trees)
		for (auto tree : forest->GetTrees())
			tree->setHeight(terrain->GetHeightMap()[int(tree->getPostitonZ() + (tree->getPostitonX() * terrain->Resolution()))]);

	terrain->BuildMap(renderer->getDevice());
}

void App::AddTree(XMFLOAT2 CurrentPos)
{
	// Create new group of trees
	Trees * tree;
	tree = new Trees(2);

	tree->init(renderer->getDevice(), renderer->getDeviceContext(), wnd);
	
	// Random offset for position
	float x = std::abs(islandSettings.size.x / 4 - (treeSettings.size.x + treeSettings.size.y));
	float y = std::abs(islandSettings.size.x / 4 - (treeSettings.size.x + treeSettings.size.y));

	float offsetPosx = CurrentPos.x + ((rand() % 2*x) - x);
	float offsetPosy = CurrentPos.y + ((rand() % 2*y) - y);

	// Set values for trees and create them
	tree->setDensity(treeSettings.density);
	tree->CreateTrees(treeSettings.size.x + (rand() % 6) - 3, treeSettings.size.y + (rand() % 6) - 3, terrain->GetHeightMap(), terrain->Resolution(), XMFLOAT2(offsetPosx,offsetPosy));
	trees.push_back(tree);
}

void App::DrawGUI()
{  
	DrawLSystemGUI();
    DrawTerrainGUI();

    // Draw Side Panel
    if (isSettingsOpen)
    {      
        // Check what setting window need to be open
        static TerrainPlane::PerlinSettings perlinSettings;
        
        switch (openSettings)
        {
        case App::Base: // Draw Base Window
            ImGui::Begin("Perlin Settings");

            ImGui::SliderFloat("Scale", &perlinSettings.scale, 0, 500);
            ImGui::SliderFloat("Frequency", &perlinSettings.frequency, 0, 1);
            ImGui::SliderFloat("Offset", &perlinSettings.offset, 0, 20);
            ImGui::SliderInt("Octaves", &perlinSettings.octaves, 0, 10);

            terrain->SetPerlin(perlinSettings);

            break;
        case App::Island: // Draw Island Window
            ImGui::Begin("Island settings");
            
            ImGui::SliderFloat("Size X", &islandSettings.size.x, 50, 300);
            ImGui::SliderFloat("Size Y", &islandSettings.size.y, 50, 300);
			ImGui::SliderFloat2("Center Y", &islandSettings.centre.x, 0, 500);
            ImGui::SliderFloat("Height", &islandSettings.height, 0, 100);

            terrain->SetIsland(islandSettings);

            break;
        case App::Tree: // Draw Tree Window
            ImGui::Begin("Tree Settings");

            ImGui::SliderFloat("Density", &treeSettings.density, 0.1f, 50.f);
            ImGui::SliderFloat2("Size", &treeSettings.size.x, 10.f, 100.f);

            break;
		case App::Camera: // Draw Tree Window
            ImGui::Begin("Camera Settings");

			static XMFLOAT2 speed;

			if (ImGui::SliderFloat2("Camera Speed", &speed.x, 0.0f, 100.f))
				cam->setMoveSpeed(speed.x,speed.y);

            break;
        default:
            // Start Default Window
            ImGui::Begin("Terrain Generation");
            break;
        }
        
        // Button to close window
		if (ImGui::Button("Close"))
			isSettingsOpen = false;

        ImGui::End();
    }
}

void App::DrawLSystemGUI()
{
	// Draw a new tab to show L-system variables
	ImGui::Begin("L-system");

	ImGui::Text("L-System Axiom: %s", system->GetAxiom().c_str());
	ImGui::Text("L-System Current: %s", system->GetCurrentSystem().c_str());

	if (ImGui::Button("Iterate L-System"))
		system->Iterate();

	ImGui::SliderInt("Run Amount", &runAmount, 0, 100);
	ImGui::Checkbox("Reset System on run", &reset);

	if (ImGui::Button("Run for x L-System"))
		system->Run(runAmount, reset);

	if (ImGui::Button("Regen L-System"))
		RegenSystem();

	ImGui::NewLine();
	ImGui::Text("Standard Rules");

	for (auto const& rule : system->GetRules())
		ImGui::Text((std::string(&rule.first, sizeof(rule.first)) + " -> " + std::string(rule.second.successor)).c_str());

	ImGui::NewLine();
	ImGui::Text("Stochastic Rules");

	for (auto const& rule : system->GetStochRules())	
		for (auto const& successor : rule.second.successors)
			ImGui::Text((std::string(&rule.first, sizeof(rule.first)) + " -> " + std::string(successor.first) + " (" + std::to_string(successor.second) + ")").c_str());

	ImGui::NewLine();
	ImGui::Text("Context Rules");

	for (auto const& rule : system->GetContextRules())
		ImGui::Text((std::string(&rule.first, sizeof(rule.first)) + " -> " + std::string(rule.second.successor)).c_str());

	ImGui::End();
}

void App::DrawTerrainGUI()
{
	// Draw a new tab to show terrain variables
	ImGui::Begin("Terrain");

	if (ImGui::SliderInt("Island Amount", &islandAmount, 0, 5))
	{
		islandSettings.amount = islandAmount;
		terrain->SetIsland(islandSettings);
	}

	if (ImGui::Button("Island Settings"))
	{
		isSettingsOpen = true;
		openSettings = Island;
	}

	ImGui::SliderInt("Tree Amount", &treeAmount, 0, 5);

	if (ImGui::Button("Tree Settings"))
	{
		isSettingsOpen = true;
		openSettings = Tree;
	}

	ImGui::SliderInt("Mountain Amount", &mountainAmount, 0, 5);

	if (ImGui::Button("Mountain Settings"))
	{
		isSettingsOpen = true;
		openSettings = Base;
	}

	if (ImGui::Button("Camera Settings"))
	{
		isSettingsOpen = true;
		openSettings = OpenSetings::Camera;
	}

	ImGui::NewLine();

	static char seed[20] = "";

	ImGui::InputTextWithHint("Seed", "Input seed here", seed, sizeof(seed));

	if (ImGui::Button("Regen Map"))
		GenerateTerrain();

	ImGui::End();
}
