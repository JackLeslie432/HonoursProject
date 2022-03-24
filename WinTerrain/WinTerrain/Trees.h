#pragma once

#include "CylinderMesh.h"
#include "SphereMesh.h"
#include "LSystem.h"
#include "Shader.h"

class Trees
{
public:
	// Tree variables	
	struct Branch
	{
		XMMATRIX rotMatrix;
		XMFLOAT3 posVect;
		XMFLOAT3 dirVect;
	};

	struct Tree
	{
		std::vector<Branch*> branches;
		std::vector<XMFLOAT3> treeBalls;
		
		//Position
		float x, y, z;
		
		void setPosition(float inx, float inz) { x = inx; z = inz; };
		float getPostitonX() { return x; };
		float getPostitonY() { return y; };
		float getPostitonZ() { return z; };
		void setHeight(float iny) { y = iny; };
	};

	Trees(int iteration);
	Trees();
	~Trees();

	void init(ID3D11Device* device, ID3D11DeviceContext* deviceCon, HWND hwnd);
	void render(XMMATRIX worldMatrix, XMMATRIX viewMatrix, XMMATRIX projectionMatrix, ID3D11ShaderResourceView* tex, ID3D11ShaderResourceView* leaveTex);

	void CreateTress(int height, int width, float* heightMap, int resolution);
	void RemoveTrees();

	void setIterations(int it) { iterations = it; };
	int getIterations() { return iterations; };

	void setBranchRes(int it) ;
	int getBranchRes() { return branchRes; };

	void setLeaveRes(int it) ;
	int getLeaveRes() { return leaveRes; };

	void setDensity(float in) { density = in; };
	float getDensity() { return density; };

private:
	
	struct vec2f
	{
		float x;
		float y;

		// Constructor
		vec2f() {};
		vec2f(float inx, float iny) {
			x = inx;
			y = iny;
		}

		// Operator overloads
		vec2f& operator = (const vec2f& in)
		{
			x = in.x;
			y = in.y;
			return *this;
		}

		vec2f& operator += (const vec2f& in)
		{
			x += in.x;
			y += in.y;
			return *this;
		}

		vec2f& operator *= (const float& in)
		{
			x *= in;
			y *= in;
			return *this;
		}
	};

	// init grid and active list for poission disc
	std::vector<vec2f> grid;
	std::vector<vec2f> active;
	
	CylinderMesh* branchMesh;
	SphereMesh* treeLeave;

	std::vector<Tree*> trees;

	// Shader and device
	ID3D11DeviceContext* deviceContext;
	ID3D11Device* device;
	Shader* shader;

	// L-system variables
	LSystem	lSystem;
	int	iterations;	

	float density = 4;

	int branchRes = 5;
	int leaveRes = 5;
	
	// Member functions
	void BuildTree(Tree* tree);
	void poissonDisc(int height, int width);
};

