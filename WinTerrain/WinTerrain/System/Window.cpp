#include "Window.h"

Window::Window(App* in_app, HINSTANCE in_hInstance, int screenWidth, int screenHeight)
{
	app = in_app;
	hInstance = in_hInstance;

	StartWindow(screenWidth, screenHeight);

	app->Init(wnd, &input);

	_Application_Handle = this;
}

Window::~Window()
{
	_Application_Handle = NULL;

}

int Window::StartWindow(int screenWidth, int screenHeight)
{
	const wchar_t APP_NAME[] = L"Terrain Generation";

	WNDCLASS wc{};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = APP_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowExW(
		0, APP_NAME, APP_NAME, WS_OVERLAPPEDWINDOW,
		// Default position and size
		CW_USEDEFAULT, CW_USEDEFAULT, screenWidth, screenHeight,
		NULL, NULL, hInstance, NULL        
	);

	// NUll check window
	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, SW_SHOW);

	// Set window as focus after showing
	SetForegroundWindow(hwnd);
	SetFocus(hwnd);

	wnd = hwnd;
	return 0;
}

LRESULT Window::WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);

		FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

		EndPaint(hwnd, &ps);
	}
	case WM_KEYDOWN:
	{
		_Application_Handle->input.SetKeyDown(wParam);
		break;
	}
	case WM_KEYUP:
	{
		_Application_Handle->input.SetKeyUp(wParam);
		break;
	} 
	case WM_MOUSEMOVE:
	{
		_Application_Handle->input.setMouseX(LOWORD(lParam));
		_Application_Handle->input.setMouseY(HIWORD(lParam));
		break;
	}
	case WM_LBUTTONDOWN:
	{
		_Application_Handle->input.setLeftMouse(true);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		_Application_Handle->input.setRightMouse(true);
		break;
	}
	case WM_LBUTTONUP:
	{
		_Application_Handle->input.setLeftMouse(false);
		break;
	}
	case WM_RBUTTONUP:
	{
		_Application_Handle->input.setRightMouse(false);
		break;
	}
		return 0;
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Window::Run()
{
	// The render loop is controlled here.
	bool bGotMsg;
	MSG  msg;
	msg.message = WM_NULL;
	PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

	while (WM_QUIT != msg.message)
	{
		// Process window events.
		// Use PeekMessage() so we can use idle time to render the scene. 
		bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

		if (bGotMsg)
		{
			// Translate and dispatch the message
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.frame();
			// Update the scene.
			app->Frame(timer.getTime());

			// Render frames during idle time (when no messages are waiting).
			//renderer->Render();

			// Present the frame to the screen.
			//deviceResources->Present();
		}
	}

}