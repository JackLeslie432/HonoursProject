#include "Trees.h"

Trees::Trees(int iteration) :
	lSystem("A")
{
	iterations = iteration;
}

Trees::~Trees()
{
	if (deviceContext)
	{
		delete deviceContext;
		deviceContext = NULL;
	}

	if (branchMesh)
	{
		delete branchMesh;
		branchMesh = nullptr;
	}

	if (treeLeave)
	{
		delete treeLeave;
		treeLeave = nullptr;
	}

	if (shader)
	{
		delete shader;
		shader = nullptr;
	}

	for (auto tree : trees)
	{
		for (auto branch : tree->branches)
		{
			if (branch)
			{
				branch = nullptr;
				delete branch;
			}
		}

		if (tree)
		{
			delete tree;
			tree = nullptr;
		}
	}
}

void Trees::init(ID3D11Device* dev, ID3D11DeviceContext* deviceCon, HWND hwnd)
{
	deviceContext = deviceCon;
	device = dev;

	shader = new Shader(device);
	shader->InitShader(L"color_vs.cso", L"color_ps.cso");

	branchMesh = new CylinderMesh(device, deviceContext, 2);
	treeLeave = new SphereMesh(device, deviceContext, 2);

	//Build the LSystem
	lSystem.SetAxiom("FA");
	lSystem.AddRule('A', "[&FA][&/FA][&RFA]");
	lSystem.Run(iterations, true);
}

void Trees::render(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* branchTex, ID3D11ShaderResourceView* leaveTex)
{
	XMMATRIX orginalWorld = worldMatrix;

	for (auto tree : trees)
	{
		for (auto const branch : tree->branches)
		{
			worldMatrix = orginalWorld;
			float scaling = branch->dirVect.y * 0.1;
			worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(scaling, scaling, scaling));
			
			//if (branch->rotMatrix != NULL)
				//worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, branch->rotMatrix);
			worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(branch->posVect.x, branch->posVect.y, branch->posVect.z));
			worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(tree->x, tree->y,tree->z ));

			shader->SetShaderParams(deviceContext, worldMatrix, viewMatrix, projectionMatrix, branchTex); // Add shader for trees
			branchMesh->sendData(deviceContext);
			shader->Render(deviceContext, branchMesh->getIndexCount());
		}

		for (auto const leave : tree->treeBalls)
		{
			worldMatrix = orginalWorld;

			float scaling = 0.3;
			worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixScaling(scaling, scaling, scaling));
			worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(leave.x, leave.y, leave.z));
			worldMatrix = DirectX::XMMatrixMultiply(worldMatrix, DirectX::XMMatrixTranslation(tree->x, tree->y, tree->z));

			shader->SetShaderParams(deviceContext, worldMatrix, viewMatrix, projectionMatrix, leaveTex);
			treeLeave->sendData(deviceContext);
			shader->Render(deviceContext, treeLeave->getIndexCount());
		}
	}
}

void Trees::CreateTrees(int height, int width, float* heightMap, int resolution, XMFLOAT2 startPos)
{
	RemoveTrees();
	PoissonDisc(height, width);

	for (auto gridPoint : grid)
	{
		if (gridPoint.x)
		{
			int x = gridPoint.x + startPos.x;
			int y = gridPoint.y + startPos.y;

			x -= width / 2;
			y -= height / 2;

			Tree* tree = new Tree();

			if (x > resolution || y > resolution || x < 0 || y < 0)
				continue;

			tree->setHeight(heightMap[y + (x * resolution)]);
			tree->setPosition(x, y);
			
			BuildTree(tree);

			trees.push_back(tree);
		}
	}
}

void Trees::RemoveTrees()
{
	for (auto tree : trees)
	{
		tree->branches.clear();
		tree->treeBalls.clear();
	}
	trees.clear();
}

void Trees::setBranchRes(int it)
{
	 branchRes = it;
	 branchMesh = nullptr;
	 delete branchMesh;
	 branchMesh = new CylinderMesh(device, deviceContext, branchRes);
}

void Trees::setLeaveRes(int it)
{
	leaveRes = it;
	treeLeave = nullptr;
	delete treeLeave;
	treeLeave = new SphereMesh(device, deviceContext, leaveRes);
}

void Trees::Destroy()
{
	if (branchMesh)
		delete branchMesh;
	branchMesh = nullptr;
	
	if (treeLeave)
		delete treeLeave;
	treeLeave = nullptr;

	for (auto tree : trees)
	{
		if (tree)
			delete tree;
		tree = nullptr;
	}

	trees.clear();

	//if (deviceContext)
	//	delete deviceContext;
	//deviceContext = nullptr;
	//
	//if (device)
	//	delete device;
	//device = nullptr;
	//
	//if (shader)
	//	delete shader;
	//shader = nullptr;
}

void Trees::BuildTree(Tree* tree)
{	
	tree->branches.clear();
	tree->treeBalls.clear();
	lSystem.Run(iterations, true);

	//Get the current L-System string, right now we have a place holder
	std::string systemString = lSystem.GetCurrentSystem();;

	//Initialize some variables
	XMVECTOR pos = XMVectorReplicate(0.0f);	//Current position
	XMVECTOR dir = XMVectorSet(0, 1, 0, 1);	//Current direction

	XMVECTOR up = XMVectorSet(0, 1, 0, 1);	//Rotation axis
	XMVECTOR right = XMVectorSet(1, 0, 0, 1);	//Rotation axis
	XMMATRIX currentRotation = XMMatrixRotationRollPitchYaw(0, 0, 0);

	// Vector for storing position and rotation
	std::vector<XMVECTOR> posVector;
	std::vector<XMMATRIX> rotVector;
	std::vector<XMVECTOR> dirVector;

	int branch = 1;

	//Go through the L-System string
	for (int i = 0; i < systemString.length(); i++) {
		Branch* temp = new Branch();
		// What each constant does
		switch (systemString[i]) {
			//If it's F, make a line
		case 'F':
			XMStoreFloat3(&temp->dirVect, dir);
			XMStoreFloat3(&temp->posVect, pos);
			temp->rotMatrix = currentRotation;

			tree->branches.push_back(temp);
			pos += XMVector3Transform(dir, currentRotation);						//Move the position marker
			break;
			// Store position
		case 'A':
			XMFLOAT3 tempPos;
			XMStoreFloat3(&tempPos, pos);
			tree->treeBalls.push_back(tempPos);
			break;
		case '[':
			posVector.push_back(pos);
			rotVector.push_back(currentRotation);
			dirVector.push_back(dir);
			dir *= 0.8;
			branch++;
			break;
			// Restore position
		case ']':
			if (!posVector.empty())
			{
				pos = posVector.back();	//Current position
				posVector.pop_back();
			}

			if (!rotVector.empty())
			{
				currentRotation = rotVector.back();	//Current position
				rotVector.pop_back();
			}

			if (!dirVector.empty())
			{
				dir = dirVector.back();
				dirVector.pop_back();
			}
			branch--;
			break;
		case '&':			
			currentRotation *= XMMatrixRotationAxis(XMVector3Transform(right, currentRotation), XMConvertToRadians(rand() % 50 + 15));
			break;
		case '/':
			if (!rotVector.empty())
				currentRotation *= XMMatrixRotationAxis(XMVector3Transform(up, rotVector.back()), XMConvertToRadians(120));
			break;
		case 'R':
			if (!rotVector.empty())
				currentRotation *= XMMatrixRotationAxis(XMVector3Transform(up, rotVector.back()), XMConvertToRadians(-120));
			break;
		}
		temp = NULL;
		delete temp;
	}
}

void Trees::PoissonDisc(int height, int width)
{
	grid.clear();
	active.clear();

	// Init variables
	int k = 30; // constant for how much to check for a free space
	float r = density; // Radius

	float w = r / sqrt(2);

	if (w > width / 2) // setting a max width it the radius is too big	
		w = width / 2;		

	int cols = width / w;
	int rows = height / w;

	// Fill grid with initial NULL
	for (int i = 0; i < cols * rows; i++)
	{
		vec2f pos(NULL, NULL);
		grid.push_back(pos);
	}

	// Find start position
	int x = rand() % width - 1;
	int y = rand() % height - 1;

	int i = x / w;
	int j = y / w;

	vec2f pos(x, y);

	// Add starting point to the grid	
	if (grid.empty())
			grid.emplace_back();

	//pos = vec2f(startingPos.x, startingPos.y);
	//
	//grid[0] = pos;
	//active.push_back(pos);

	if (i + (j * cols) < grid.size())
	{
		grid[i + (j * cols)] = pos;
		active.push_back(pos);
	}
	else
	{ 
		pos = vec2f(0,0);
	
		grid[0] = pos;
	
		active.push_back(pos);
	}

	// Generate new points
	while (!active.empty())
	{
		// choose a random point in the active list
		int index = rand() % active.size();
		pos = active[index];
		bool found = false;

		// loop for the amount of attempts you want to have
		for (int n = 0; n < k; n++)
		{
			// Choose a random angle to check
			float angle = rand() % int(2.f * 3.1415f);

			vec2f sample(cos(angle), sin(angle));

			// Choose a random magnitude between r and 2r
			float mag = rand() % int(2 * r) + r;

			// Set new point to the offset of the current point
			sample *= mag;
			sample += pos;

			// Find what grid space it is in
			int col = sample.x / w;
			int row = sample.y / w;

			// Check within range and the grid doesn't already have a point assigned to it
			if (col > -1 && row > -1 && col < cols && row < rows && !grid[col + row * cols].x)
			{
				bool ok = true;
				// Nested loop for all points around it
				for (int i = -1; i <= 1; i++)
				{
					for (int j = -1; j <= 1; j++)
					{
						if ((col + i) + (row + j) * cols > -1 && (col + i) + (row + j) * cols < cols)
						{
							vec2f neighbor = grid[(col + i) + (row + j) * cols];
							// Find the distance to the new point
							float d = sqrt(((sample.x - neighbor.x) * (sample.x - neighbor.x)) + ((sample.y - neighbor.y) * (sample.y - neighbor.y)));
							// if the distance is smaller than the radius then throw it away
							if (d < r)
							{
								ok = false;
							}
						}
					}
				}

				// if the point is out with the radius
				if (ok)
				{
					// found a point
					found = true;
					// set the grid to the new pint that has been calculated
					grid[col + row * cols] = sample;
					// add the new point to the active list
					active.push_back(sample);
				}
			}
		}

		if (!found)
		{
			active.erase(active.begin() + index);
		}
	}
}