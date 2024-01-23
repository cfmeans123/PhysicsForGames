#pragma once

#include "Platform/Singleton.h"
#include "Visual/OpenGL/OpenGL.h"

namespace jm
{
	class ImGuiContext;
}

namespace jm::Platform
{
	class Window;
}
namespace jm::OpenGL
{
	class Rasterizer;
	class Memory;
}

namespace jm::Rendering
{
	class Context : singleton<Context>
	{
	public:

		Context(Platform::Window& window);

		std::unique_ptr<OpenGL::Rasterizer> RasterizerImpl;
		std::unique_ptr<OpenGL::Memory> RasterizerMemory;
		std::unique_ptr<ImGuiContext> ImGuiContextPtr;
	};
}
