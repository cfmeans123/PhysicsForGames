#pragma once

#include "Visual/RenderingContext.h"
#include "Visual/Visual.h"
#include "Math/Camera.h"

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

	public:
		Graphics(Platform::Window& window, entity_registry& registry);
		~Graphics();

		Platform::MessageHandler* GetMessageHandler();

		void Draw3D(math::camera3<f32> const& camera, math::vector3_f32 const& clearColour, std::function<void()>&& imguiFrame);
	};
}