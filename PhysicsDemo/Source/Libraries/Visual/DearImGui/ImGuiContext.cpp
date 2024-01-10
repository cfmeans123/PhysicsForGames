#include "ImGuiContext.h"

#include "OpenGL/OpenGL.h"
#include "DearImGui/backends/imgui_impl_opengl3.h"

#if JM_ON_WINDOWS
#include "DearImGui/backends/imgui_impl_win32.h"
#else
#error unsupported OS!
#endif

#include "Platform/OSWindow.h"

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace jm
{
	void RunImGuiFrame(std::function<void()>&& frameFunction)
	{
		ImGui_ImplOpenGL3_NewFrame();

#if JM_ON_WINDOWS
		ImGui_ImplWin32_NewFrame();
#else
#error unsupported OS!
#endif

		ImGui::NewFrame();

		frameFunction();

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	bool ImGuiInputHandler::ProcessSystemMessage(MSG& systemMessage)
	{
		ImGui_ImplWin32_WndProcHandler(systemMessage.hwnd, systemMessage.message, systemMessage.wParam, systemMessage.lParam);
		ImGuiIO& io = ImGui::GetIO();
		return io.WantCaptureMouse || io.WantCaptureKeyboard;
	}

	std::atomic_flag singleton<ImGuiContext>::s_instance_flag;

#if JM_ON_WINDOWS
	ImGuiContext::ImGuiContext(const OpenGL::Rasterizer& apiPipeline, HWND nativeWindowHandle)
#else
#error unsupported OS!
#endif
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO();

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

		ImGui::StyleColorsDark();

#if JM_ON_WINDOWS
		ImGui_ImplWin32_Init(nativeWindowHandle);
#else
#error unsupported OS!
#endif

		ImGui_ImplOpenGL3_Init(apiPipeline.GLSLVersion);
	}

	ImGuiContext::~ImGuiContext()
	{
		ImGui_ImplOpenGL3_Shutdown();
#if JM_ON_WINDOWS
		ImGui_ImplWin32_Shutdown();
#else
#error unsupported OS!
#endif
		ImGui::DestroyContext();
	}

	void ImGuiContext::RunFrame(std::function<void()>&& frameFunction) const
	{
		RunImGuiFrame(std::move(frameFunction));
	}
}