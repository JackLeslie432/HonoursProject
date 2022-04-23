#include "App.h"
#include "backends/imgui_impl_dx11.h"
#include "backends/imgui_impl_win32.h"
#include "StochasticRule.h"

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

    // Load in textures to be used
    textureMgr->LoadTexture(L"grass", L"res/grass.png");
    textureMgr->LoadTexture(L"bark", L"res/bark.png");
    textureMgr->LoadTexture(L"leaves", L"res/leaves.png");

    // Setup a new camera
    cam = new FPCamera(input, 1200, 675, hwnd);
    cam->setPosition(0.0f, 0.0f, -10.0f);
    cam->update();

    InitLSystem();

	tree = new Trees(2);

	tree->init(renderer->getDevice(), renderer->getDeviceContext(), hwnd);    
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

	tree->render(worldMatrix, viewMatrix, projectionMatrix,  textureMgr->GetTexture(L"bark"), textureMgr->GetTexture(L"leaves"));

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

	// Adding normal rules
    std::string islands("[");
    for (int i = 0; i < islandAmount; i++)
        islands.append("I");
    islands.append("]");

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

	// Adding normal rules
	std::string islands("[");
	for (int i = 0; i < islandAmount; i++)
		islands.append("I");
	islands.append("]");

	system->AddRule('O', islands);

	// Add stochastic rules
	StochasticRule stochRules;

	stochRules.Add("IT", 50);
	stochRules.Add("IM", 50);

	system->AddStochRule('I', stochRules.GetRules());

    return true;
}

void App::DrawGUI()
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
            ImGui::Text((std::string(&rule.first, sizeof(rule.first)) + " -> " +std::string(rule.second.successor)).c_str());

        ImGui::NewLine();
        ImGui::Text("Stochastic Rules");

        for (auto const& rule : system->GetStochRules())
        {
            for (auto const& successor : rule.second.successors)            
                ImGui::Text((std::string(&rule.first, sizeof(rule.first)) + " -> " + std::string(successor.first) + " (" + std::to_string(successor.second) + ")").c_str());            
        }

        ImGui::NewLine();
        ImGui::Text("Context Rules");

        for (auto const& rule : system->GetContextRules())		
            ImGui::Text((std::string(&rule.first, sizeof(rule.first)) + " -> " +std::string(rule.second.successor)).c_str());
        

    ImGui::End();

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

        ImGui::NewLine();

        static char seed[20] = "";

        ImGui::InputTextWithHint("Seed", "Input seed here", seed, sizeof(seed));

        if (ImGui::Button("Regen Map"))
        {
            terrain->Regenerate(nullptr, renderer->getDevice());

            for (auto c : system->GetCurrentSystem())
            {
                switch (c)
                {
                case 'I':
                    terrain->CreateIsland();
                    break;
                case '[':
                    break;
                case ']':
                    break;
                case 'T':
                    break;
                case 'M':
                    break;
                default:
                    break;
                }
            }


            tree->setDensity(islandSettings.size.x/50);
            if (!terrain->GetIslandCenter().empty())
                tree->CreateTrees(islandSettings.size.x/2, islandSettings.size.x/2, terrain->GetHeightMap(), terrain->Resolution(),terrain->GetIslandCenter().front());
        }

	ImGui::End();

    // Draw Side Panel
    if (isSettingsOpen)
    {      
        // Check what setting window need to be open
        static TerrainPlane::PerlinSettings perlinSettings;
        
        switch (openSettings)
        {
        case App::Base: // Draw base window
            ImGui::Begin("Perlin Settings");

            ImGui::SliderFloat("Scale", &perlinSettings.scale, 0, 500);
            ImGui::SliderFloat("Frequency", &perlinSettings.frequency, 0, 1);
            ImGui::SliderFloat("Offset", &perlinSettings.offset, 0, 20);
            ImGui::SliderInt("Octaves", &perlinSettings.octaves, 0, 10);

            terrain->SetPerlin(perlinSettings);

            break;
        case App::Island: // Draw island window
            ImGui::Begin("Island settings");
            
            ImGui::SliderFloat("Size X", &islandSettings.size.x, 50, 300);
            ImGui::SliderFloat("Size Y", &islandSettings.size.y, 50, 300);
			ImGui::SliderFloat2("Center Y", &islandSettings.centre.x, 0, 500);
            ImGui::SliderFloat("Height", &islandSettings.height, 0, 100);

            terrain->SetIsland(islandSettings);

            break;
        case App::Tree: // Draw tree window
            ImGui::Begin("Tree Settings");
            break;
        default:
            // Start Window
            ImGui::Begin("Terrain Generation");
            break;
        }
        
        // Button to close window
		if (ImGui::Button("Close"))
			isSettingsOpen = false;

        ImGui::End();
    }
    

}
