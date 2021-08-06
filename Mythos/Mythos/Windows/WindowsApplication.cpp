#include "WindowsApplication.h"

#include "../Resource.h"
#include <SDKDDKVer.h>

WindowsApplication* WindowsApplication::winApp = NULL;

WindowsApplication::WindowsApplication(HINSTANCE& hInstance, const wchar_t* title, const wchar_t* windowClass, int nCmdShow)
{
    winApp = this;
    SetWindowLongPtr(window, GWLP_USERDATA, -21);
	this->title = title;
	this->windowClass = windowClass;
    this->instance = hInstance;
	Register(hInstance);
	Init(hInstance, nCmdShow);
}

ATOM WindowsApplication::Register(HINSTANCE& hInstance)
{
    WNDCLASSEX wcex;

    ZeroMemory(&wcex, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_VREDRAW | CS_HREDRAW;
    wcex.lpfnWndProc = this->WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszClassName = windowClass;

    return RegisterClassExW(&wcex);
}

BOOL WindowsApplication::Init(HINSTANCE& hInstance, int nCmdShow)
{
    DWORD dwstyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME;

    window = CreateWindowW(windowClass, title, dwstyle,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!window)
        return FALSE;

    SetMenu(window, NULL);
    ShowWindow(window, nCmdShow);
    UpdateWindow(window);

    return TRUE;
}

LRESULT CALLBACK WindowsApplication::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (winApp) return winApp->WindowProc(hWnd, message, wParam, lParam);
    return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND& WindowsApplication::GetWindow() { return window; }
HINSTANCE& WindowsApplication::GetInstance() { return instance; }

LRESULT CALLBACK WindowsApplication::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch(message)
    {
        case WM_DESTROY: {
            //The X button has been pressed
            PostQuitMessage(0x78);
            break;
        }
        default: return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
