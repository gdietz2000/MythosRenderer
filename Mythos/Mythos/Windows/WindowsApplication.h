#pragma once
#include <Windows.h>

class WindowsApplication 
{
public:
	//Removing the assignment operator
	WindowsApplication& operator=(const WindowsApplication&) = delete;

	//Default Constructor of the WindowsApplication Class
	WindowsApplication(HINSTANCE& hInstance, const wchar_t* title, const wchar_t* windowClass, int nCmdShow);
	//Static function that refers to the private window proc
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND& GetWindow();
	HINSTANCE& GetInstance();
private:
	//Registers the window class
	ATOM Register(HINSTANCE& hInstance);
	//Initializes the window
	BOOL Init(HINSTANCE& hInstance, int nCmdShow);

	//The callback function that occurs whenever something happens
	LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	//The title of the window
	const wchar_t* title;
	const wchar_t* windowClass;

	static WindowsApplication* winApp;

	HINSTANCE instance;
	HWND window;
};