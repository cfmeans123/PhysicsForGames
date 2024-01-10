#pragma once

#include "Platform/PlatformCore.h"
#include "Platform/OS.h"
#include "Platform/Window.h"

#include "MeshData.h"

#include "FunctionBindings.h" //TODO: should be hidden

#include "Math/MathTypes.h"

#include <map>

namespace jm::OpenGL
{
	void CheckError();
	void CheckFrameBuffer();

	class Rasterizer
	{
	public:
#if JM_ON_WINDOWS
		Rasterizer(HWND nativeWindowHandle, Platform::PixelArea area);
#else
#error "UNSUPPORTED OS!"
#endif // JM_WINDOWS

		~Rasterizer();

		void PrepareRenderBuffer(math::vector3_f32 const& colour);
		void UpdateRenderBuffer();

		cstring const GLSLVersion = "#version 450 core";

	private:

#if JM_ON_WINDOWS
		HDC   GLDeviceContext = NULL;
		HGLRC GLRenderingContext = NULL;

		HWND WindowHandle = NULL;
#else
#error "UNSUPPORTED OS!"
#endif // JM_WINDOWS
	};

	using InputLayoutHandle = uSize;

	using InputBufferHandle = uSize;

	class Memory
	{
	public:
		InputLayoutHandle createInputLayout(const Visual::InputLayout& inputLayout);
		void destroyInputLayout(InputLayoutHandle& inputLayoutHandle);

		InputBufferHandle createInputBuffer(
			InputLayoutHandle inputLayoutHandle,
			const byte_list& inputData);
		void destroyInputBuffer(InputLayoutHandle inputLayoutHandle, InputBufferHandle& bufferHandle);

	private:

		struct LayoutState
		{
			Visual::InputLayout layout;
			std::vector<GLuint> VBO{};
		};

		std::map<GLuint, LayoutState> VAOStates;
	};
}
