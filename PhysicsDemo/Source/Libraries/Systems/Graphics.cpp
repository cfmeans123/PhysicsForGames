#include "Graphics.h"


#include "Platform/WindowedApplication.h"
#include "Visual/DearImGui/ImGuiContext.h"
#include "Visual/VisualGeometry.h"

namespace jm::System
{
	Graphics::Graphics(Platform::Window& window, entity_registry& registry)
		: Renderer(window)
		, EntityRegistry(registry)
		, Program(R"(
			#version 330 core
			layout (location = 0) in vec3 inPosition;
			layout (location = 1) in vec3 inColour;
			
			out vec3 outColour;

			uniform mat4 projectionView;

			void main()
			{
				gl_Position = projectionView * vec4(inPosition, 1.0);
				outColour = inColour;
			}
			)", R"(
			#version 330 core
			in vec3 outColour;
			
			out vec4 FragColor;

			void main()
			{
				FragColor = vec4(outColour, 1.0f);
			}
			)")
	{
		Visual::InputLayout layout{ {3, 3} };

		byte_list inputVertexData;
		{
			auto cubeVertexData = Visual::GenerateCube(layout);
			inputVertexData.insert(inputVertexData.end(), cubeVertexData.data.begin(), cubeVertexData.data.end());
			cubeVertices = static_cast<GLsizei>(cubeVertexData.size);
		}

		inputLayoutHandle = Renderer.RasterizerMemory->createInputLayout(layout);
		inputBufferHandle = Renderer.RasterizerMemory->createInputBuffer(inputLayoutHandle, inputVertexData);

		glEnable(GL_DEPTH_TEST);
	}

	Graphics::~Graphics()
	{
		Renderer.RasterizerMemory->destroyInputBuffer(inputLayoutHandle, inputBufferHandle);
		Renderer.RasterizerMemory->destroyInputLayout(inputLayoutHandle);
	}

	Platform::MessageHandler* Graphics::GetMessageHandler()
	{
		return Renderer.ImGuiContextPtr->GetMessageHandler();
	}

	void Graphics::Draw3D(math::camera3<f32> const& camera, math::vector3_f32 const& clearColour, std::function<void()> && imguiFrame)
	{
		Renderer.RasterizerImpl->PrepareRenderBuffer(clearColour);

		Program.SetUniform("projectionView", camera.get_perspective_transform() * camera.get_view_transform());
		{
			glBindVertexArray(static_cast<GLuint>(inputLayoutHandle));
			GLsizei start = 0;

			glDrawArrays(GL_TRIANGLES, start, cubeVertices);
		}

		Renderer.ImGuiContextPtr->RunFrame(std::move(imguiFrame));

		Renderer.RasterizerImpl->UpdateRenderBuffer();
	}
}
