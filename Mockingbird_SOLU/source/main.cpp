// D3D Project "Mockingbird"
// Author: Junshu Chen

// Introduction:



//************************************************************
//************ INCLUDES & DEFINES ****************************
//************************************************************

#include <ctime>
#include "DEMO.h"


//************************************************************
//************ WINDOWS RELATED *******************************
//************************************************************
static DEMO* gDemo = nullptr;
// ****************** BEGIN WARNING ***********************// 
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	gDemo = DEMO::GetInstance(GetModuleHandle(NULL), (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT && gDemo->Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	gDemo->ShutDown();
	return 0;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY) : { PostQuitMessage(0); }
						
						break;
	case (WM_SIZE) :
	{
		RECT rc;
		GetClientRect(hWnd, &rc);
		UINT width = rc.right - rc.left;
		UINT height = rc.bottom - rc.top;
		if (gDemo)
		{
			gDemo->ResizeWindow(width, height);
		}
		break;
	}

	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}
//********************* END WARNING ************************//