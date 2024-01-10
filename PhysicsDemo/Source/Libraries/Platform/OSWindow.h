#pragma once

#include "OS.h"

#include "Window.h"

namespace jm::Platform
{
	struct WindowsWindow : Window
	{
		WindowsWindow(const RuntimeContext& runtimeContext, WindowParameters parameters);

		virtual ~WindowsWindow() override;

		HWND Handle = NULL;

	private:

		friend LRESULT WindowsProcessMessage(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	};
}
