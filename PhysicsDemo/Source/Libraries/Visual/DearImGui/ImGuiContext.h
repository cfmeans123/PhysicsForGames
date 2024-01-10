#pragma once

#include "Platform/WindowedApplication.h"
#include "Platform/Singleton.h"

#include <functional>

namespace jm::OpenGL
{
	class Rasterizer;
}

namespace jm
{
	struct ImGuiInputHandler : Platform::MessageHandler
	{
		virtual bool ProcessSystemMessage(MSG& systemMessage) override;
	};

	class ImGuiContext : singleton<ImGuiContext>
	{
		ImGuiInputHandler MessageHandler;

	public:

#if JM_ON_WINDOWS
		ImGuiContext(const OpenGL::Rasterizer& apiPipeline, HWND nativeWindowHandle);
#else
#error unsupported OS!
#endif
		~ImGuiContext();

		void RunFrame(std::function<void()>&& frameFunction) const;

		Platform::MessageHandler* GetMessageHandler()
		{
			return &MessageHandler;
		}
	};
}
