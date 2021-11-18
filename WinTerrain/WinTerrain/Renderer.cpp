#include "Renderer.h"

Renderer::Renderer(HWND hwnd)
{
	wnd = &hwnd;

	CreateDeviceResources();
	CreateSwapchain();
	CreateRenderTargetView();
	CreateDepthStencil();
}

Renderer::~Renderer()
{
	if (device)
	{
		device->Release();
		device = 0;
	}

	if (deviceContext)
	{
		deviceContext->Release();
		deviceContext = 0;
	}
	
	if (swapChain)
	{
		swapChain->Release();
		swapChain = 0;
	}
	
	if (factory)
	{
		factory->Release();
		factory = 0;
	}

	if (renderTargetView)
	{
		renderTargetView->Release();
		renderTargetView = 0;
	}
	
	if (depthStencilView)
	{
		depthStencilView->Release();
		depthStencilView = 0;
	}
}

// ------ GETTERS -----
ID3D11Device* Renderer::getDevice()
{
	return device;
}


ID3D11DeviceContext* Renderer::getDeviceContext()
{
	return deviceContext;
}

XMMATRIX Renderer::getProjectionMatrix()
{
	return projectionMatrix;
}


XMMATRIX Renderer::getWorldMatrix()
{
	return worldMatrix;
}


XMMATRIX Renderer::getOrthoMatrix()
{
	return orthoMatrix;
}

void Renderer::beginScene(float red, float green, float blue, float alpha)
{
	float color[4];

	color[0] = red;
	color[1] = green;
	color[2] = blue;
	color[3] = alpha;

	deviceContext->ClearRenderTargetView(renderTargetView, color);
	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	return;
}


void Renderer::endScene()
{
	swapChain->Present(1, 0);

	return;
}

void Renderer::SetBackBufferRenderTarget()
{
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
	return;
}

// Your initialise will create a local viewport variable, and you can swap it to this one
void Renderer::resetViewport()
{
	deviceContext->RSSetViewports(1, &viewport);
	return;
}

void Renderer::CreateRenderTargetView()
{
	ID3D11Texture2D* backBufferPtr;
	// Configure back buffer
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	device->CreateRenderTargetView(backBufferPtr, NULL, &renderTargetView);
	backBufferPtr->Release();
	backBufferPtr = 0;
}

void Renderer::CreateDeviceResources()
{
    HRESULT hr = S_OK;

    D3D_FEATURE_LEVEL levels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_11_1
    };


    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(DEBUG) || defined(_DEBUG)
    deviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    hr = D3D11CreateDevice(
        nullptr,						// Specify nullptr to use the default adapter.
        D3D_DRIVER_TYPE_HARDWARE,		// Create a device using the hardware graphics driver.
        0, deviceFlags,
		levels, ARRAYSIZE(levels), 
		D3D11_SDK_VERSION,
        &device, NULL, &deviceContext	// Returns                 
    );

    if (FAILED(hr))
	{ }

    //return hr;
}

void Renderer::CreateSwapchain()
{
	HRESULT hr = S_OK;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;

	// Initialise the swap chain description.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = 1200;
	swapChainDesc.BufferDesc.Height = 675;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = *wnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.Windowed = true;

	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Flags = 0;

	// swap chain
	hr = factory->CreateSwapChain(device, &swapChainDesc, &swapChain);

}

void Renderer::CreateDepthStencil()
{
	D3D11_DEPTH_STENCIL_DESC depthDesc;

	ZeroMemory(&depthDesc, sizeof(depthDesc));

	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthDesc.StencilEnable = true;
	depthDesc.StencilReadMask = 0xFF;
	depthDesc.StencilWriteMask = 0xFF;
	depthDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	depthDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	device->CreateDepthStencilState(&depthDesc, &depthStencilState);
	deviceContext->OMSetDepthStencilState(depthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC viewDesc;

	ZeroMemory(&viewDesc, sizeof(viewDesc));
	viewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	viewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	viewDesc.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(depthStencil, &viewDesc, &depthStencilView);
	deviceContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);
}
