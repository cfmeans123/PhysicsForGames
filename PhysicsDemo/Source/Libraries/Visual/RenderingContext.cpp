#include "RenderingContext.h"

#include "Platform/OSWindow.h"

#include "DearImGui/ImGuiContext.h"

#include "OpenGL/OpenGL.h"


namespace jm
{
	std::atomic_flag Rendering::Context::s_instance_flag;
}

namespace jm::Rendering
{
	Context::Context(Platform::Window& window)
	{
#if JM_ON_WINDOWS
		Platform::WindowsWindow& systemWindow = static_cast<Platform::WindowsWindow&>(window);
		std::unique_ptr<OpenGL::Rasterizer> pipeline = std::make_unique<OpenGL::Rasterizer>(systemWindow.Handle, systemWindow.GetArea());
		std::unique_ptr<OpenGL::Memory> memory = std::make_unique<OpenGL::Memory>();
#else
#error UNSUPPORTED OS!
#endif

		RasterizerMemory = std::move(memory);
		ImGuiContextPtr = std::make_unique<ImGuiContext>(*pipeline, systemWindow.Handle);
		RasterizerImpl = std::move(pipeline);
	}
}
