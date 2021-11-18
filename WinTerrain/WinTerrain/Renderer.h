#pragma comment( lib, "user32" )          // link against the win32 library
#pragma comment( lib, "d3d11.lib" )       // direct3D library
#pragma comment( lib, "dxgi.lib" )        // directx graphics interface
#pragma comment( lib, "d3dcompiler.lib" ) // shader compiler

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>
#include <string>

using namespace DirectX;

class Renderer
{

public:

	Renderer(HWND);
	~Renderer();

	void beginScene(float r, float g, float b, float a);
	void endScene();

	void CreateRenderTargetView();
	void SetBackBufferRenderTarget();
	void resetViewport();

	ID3D11Device* getDevice();	
	ID3D11DeviceContext* getDeviceContext(); 

	XMMATRIX getProjectionMatrix();
	XMMATRIX getWorldMatrix();
	XMMATRIX getOrthoMatrix();

private:
	void CreateDeviceResources();
	void CreateSwapchain();
	void CreateDepthStencil();

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;

	IDXGISwapChain* swapChain;
	HWND* wnd;
	IDXGIFactory1* factory;

	ID3D11RenderTargetView* renderTargetView;
	ID3D11DepthStencilView* depthStencilView;
	ID3D11DepthStencilState* depthStencilState;
	ID3D11Texture2D* depthStencil;
	D3D11_VIEWPORT viewport;

	XMMATRIX projectionMatrix;
	XMMATRIX worldMatrix;
	XMMATRIX orthoMatrix;

};

