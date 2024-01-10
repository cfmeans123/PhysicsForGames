#include "WindowedApplication.h"

#include "OSWindow.h"
#include "PlatformDebug.h"

#include <algorithm>

namespace jm::Platform
{
	constexpr int maxMessagesPerFrame = 5;

	int LoopedApplication::Run()
	{
		OnStartLoop();

		while (Running)
		{
			BeforeRunLoop();
			RunLoop();
			AfterRunLoop();
		}

		OnStopLoop();

		return ExitCode();
	}

	WindowedApplication::WindowedApplication(const RuntimeContext& context, WindowParameters parameters)
		: LoopedApplication{context}
		, window(std::make_unique<WindowsWindow>(std::cref(context), parameters))
	{
	}

	WindowedApplication::~WindowedApplication()
	{
		JM_PLATFORM_ASSERT(handlers.empty());
	}

	void WindowedApplication::BeforeRunLoop()
	{
		if (window->WasClosed())
		{
			window = nullptr;
			Running = false; //no windows, no reason to keep going
		}
	}

	void WindowedApplication::AfterRunLoop()
	{
		MSG currentMSG;
		ZeroMemory(&currentMSG, sizeof(MSG));

		int messagesReceived = 0;
		while (messagesReceived < maxMessagesPerFrame
			&& PeekMessage(&currentMSG, NULL, 0, 0, PM_REMOVE))
		{
			switch (currentMSG.message)
			{
			case WM_DESTROY:
			case WM_QUIT:
			{
				PostQuitMessage(0);
				Running = false;
				currentErrorCode = static_cast<int>(currentMSG.wParam);
				return;
			}
			}

			for (auto device : handlers)
			{
				if (device->ProcessSystemMessage(currentMSG))
				{
					break;
				}
			}

			TranslateMessage(&currentMSG);
			DispatchMessage(&currentMSG);

			messagesReceived++;
		}
	}

	int WindowedApplication::ExitCode() const
	{
		return currentErrorCode;
	}

	void WindowedApplication::AddMessageHandler(MessageHandler* handler)
	{
		handlers.push_back(handler);
	}

	void WindowedApplication::RemoveMessageHandler(MessageHandler* handler)
	{
		auto predicate = [handler](MessageHandler* someHandler)
		{
			return someHandler == handler;
		};
		handlers.erase(std::remove_if(handlers.begin(), handlers.end(), predicate));
	}
}
