#include "OSWindow.h"

#include "Application.h"
#include "PlatformDebug.h"

#include <map>

namespace jm::Platform
{
	static TCHAR windowClass[] = TEXT("SimulacraWindow");

	WindowsWindow* GetPointerFromLPARAM(LPARAM lParam)
	{
		//expects "this" pointer passed into CreateWindowFunction
		CREATESTRUCTA* createStruct = reinterpret_cast<CREATESTRUCTA*>(lParam);
		return reinterpret_cast<WindowsWindow*>(createStruct->lpCreateParams);
	}

	LRESULT WindowsProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		static std::map<HWND, WindowsWindow*> windowLookup;

		if (windowLookup.contains(hwnd) && windowLookup[hwnd]->Handle != NULL)
		{
			WindowsWindow* window = windowLookup[hwnd];

			switch (uMsg)
			{
			case WM_DESTROY:
			case WM_QUIT:
			{
				PostQuitMessage(S_OK);
				window->Close();
				return S_OK;
			}
			case WM_PAINT:
			{
				HDC hdc;
				PAINTSTRUCT ps;
				hdc = BeginPaint(hwnd, &ps);
				// Don't ever draw here!!! Unless...
				EndPaint(hwnd, &ps);
				return S_OK;
			}
			default:
				return DefWindowProc(hwnd, uMsg, wParam, lParam);
			}
		}
		else
		{
			if (WM_NCCREATE == uMsg) //first message to creating window
			{
				JM_PLATFORM_ASSERT(!windowLookup.contains(hwnd), "Create called twice?");
				windowLookup[hwnd] = GetPointerFromLPARAM(lParam);
			}

			return DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}

	bool CreateWindowClass(HINSTANCE runtimeInstance)
	{

		WNDCLASSEX wc
		{
			sizeof(WNDCLASSEX),
			CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
			WindowsProcessMessage,
			0,
			0,
			runtimeInstance,
			LoadIcon(runtimeInstance, IDI_APPLICATION),
			LoadCursor(NULL, IDC_ARROW),
			(HBRUSH)(COLOR_WINDOW + 1),
			NULL,
			windowClass,
			LoadIcon(runtimeInstance, IDI_APPLICATION)
		};

		if (RegisterClassEx(&wc))
		{
			return true;
		}
		else
		{
			FatalAppExit(NULL, TEXT("RegisterClassEx() failed!"));
			return false;
		}
	}

	RECT GetRectFromOriginAndArea(const ScreenPosition origin, const PixelArea area)
	{
		RECT rect;
		SetRect(&rect
			, int(origin.X) // left
			, int(origin.Y) // top
			, int(origin.X + area.Width) // right
			, int(origin.Y + area.Height)); // bottom
		return rect;
	}

	HWND CreateWindowInstance(WindowsWindow* windowPtr, HINSTANCE runtimeInstance, const ScreenPosition origin, const PixelArea area)
	{
		HWND instanceHandle = NULL;
		if (CreateWindowClass(runtimeInstance))
		{
			DWORD winStyle = WS_OVERLAPPEDWINDOW;
			DWORD winExStyle = WS_EX_OVERLAPPEDWINDOW;

			RECT rect = GetRectFromOriginAndArea(origin, area);

			AdjustWindowRect(&rect, winStyle, false);

			instanceHandle = CreateWindowEx(winExStyle,
				windowClass, windowPtr->GetName(),
				winStyle,
				rect.left, rect.top,  // adjusted x, y positions
				rect.right - rect.left, rect.bottom - rect.top,  // adjusted width and height
				NULL, NULL,
				runtimeInstance, windowPtr);
		}
		
		if (!instanceHandle)
		{
			FatalAppExit(NULL, TEXT("CreateWindowEx() failed!"));
		}

		return instanceHandle;
	}

	WindowsWindow::WindowsWindow(const RuntimeContext& runtimeContext, WindowParameters parameters)
		: Window(runtimeContext, parameters)
		, Handle(CreateWindowInstance(this, runtimeContext.Instance, parameters.Origin, parameters.Area))
	{
		if (Handle)
		{
			int iCmdShow = SW_SHOW;
			ShowWindow(Handle, iCmdShow);
			UpdateWindow(Handle);

			Open();
		}
	}

	WindowsWindow::~WindowsWindow()
	{
		AnimateWindow(Handle, 200, AW_HIDE | AW_BLEND);

		DestroyWindow(Handle);
	}
}

//REACTIVE WINDOWS PUMP (simple)
//while (GetMessage(...) > 0)
//{
//	TranslateMessage();
//	DispatchMessage();
//}
//REACTIVE WINDOWS PUMP (custom)
//while (true)
//{
//	while (PeekMessage(..., PM_REMOVE) == FALSE) {}
//
//	if (Msg.message == WM_QUIT)
//		break;
//
//	TranslateMessage();
//	DispatchMessage();
//}
//ACTIVE WINDOWS PUMP (single message)
//while (true)
//{
//	if (PeekMessage(..., PM_REMOVE) == TRUE)
//	{
//		if (Msg.message == WM_QUIT)
//			break;
//
//		TranslateMessage();
//		DispatchMessage();
//	}
//
//	// Do your stuff here
//}
//ACTIVE WINDOWS PUMP (all messages)
//while (true)
//{
//	while (PeekMessage(..., PM_REMOVE) == TRUE)
//	{
//		if (Msg.message == WM_QUIT)
//			break;
//
//		TranslateMessage();
//		DispatchMessage();
//	}
//
//	// Do your stuff here
//}
