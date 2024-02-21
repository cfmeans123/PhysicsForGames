#pragma once

#include "Visual/RenderingContext.h"
#include "Visual/Visual.h"
#include "Math/Camera.h"
#include "imgui.h"
#include "Entity.h"

namespace jm
{
	namespace Platform
	{
		class MessageHandler;
	}
}

namespace jm::System
{
	class Graphics
	{
		Rendering::Context Renderer;
		entity_registry& EntityRegistry;

		Visual::ShaderProgram Program;
		OpenGL::InputLayoutHandle inputLayoutHandle;
		OpenGL::InputBufferHandle inputBufferHandle;
		GLsizei cubeVertices;
		GLsizei sphereVertices;
		GLsizei axesVertices;

		math::vector3_f32 ClearColour;
		bool Debug3D = true;
		bool Debug2D = false;
	public:
		Graphics(Platform::Window& window, entity_registry& registry);
		~Graphics();
		
		Platform::MessageHandler* GetMessageHandler();

		void Draw3D(math::camera3<f32> const& camera, std::function<void()>&& imguiFrame);
		void ImGuiDebug();
	};
}