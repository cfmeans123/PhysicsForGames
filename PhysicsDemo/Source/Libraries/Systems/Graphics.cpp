#include "Graphics.h"

#include "Components.h"

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

			uniform mat4 model;
			uniform mat4 projectionView;

			void main()
			{
				gl_Position = projectionView * model * vec4(inPosition, 1.0);
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
		, ClearColour({ 0.2f, 0.3f, 0.3f })
	{
		Visual::InputLayout layout{ {3, 3} };

		byte_list inputVertexData;
		{
			auto cubeVertexData = Visual::GenerateCube(layout);
			inputVertexData.insert(inputVertexData.end(), cubeVertexData.data.begin(), cubeVertexData.data.end());
			cubeVertices = static_cast<GLsizei>(cubeVertexData.size);
		}
		{
			auto sphereVertexData = Visual::GenerateSphere(layout);
			inputVertexData.insert(inputVertexData.end(), sphereVertexData.data.begin(), sphereVertexData.data.end());
			sphereVertices = static_cast<GLsizei>(sphereVertexData.size);
		}
		{
			auto axesData = Visual::GenerateCoordinateAxes3(layout);
			inputVertexData.insert(inputVertexData.end(), axesData.data.begin(), axesData.data.end());
			axesVertices = static_cast<GLsizei>(axesData.size);
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

	void Graphics::Draw3D(math::camera3<f32> const& camera, std::function<void()> && imguiFrame)
	{
		auto spatial_shape_view = EntityRegistry.view<const spatial3_component, const shape_component>();

		std::vector<math::matrix44_f32> CubeInstances;
		std::vector<math::matrix44_f32> SphereInstances;
		for (auto&& [entity, spatial, shape] : spatial_shape_view.each())
		{
			switch (shape)
			{
			case shape_component::Sphere:
				SphereInstances.push_back(math::isometry_matrix3(spatial.position, spatial.orientation));
				break;
			default:  //box
				CubeInstances.push_back(math::isometry_matrix3(spatial.position, spatial.orientation));
				break;
			}
		}


		Renderer.RasterizerImpl->PrepareRenderBuffer(ClearColour);

		Program.SetUniform("projectionView", camera.get_perspective_transform() * camera.get_view_transform());
		{
			GLsizei start = 0;
			glBindVertexArray(static_cast<GLuint>(inputLayoutHandle));
			
			for (math::matrix44_f32& instance : CubeInstances)
			{
				Program.SetUniform("model", instance);
				glDrawArrays(GL_TRIANGLES, start, cubeVertices);
			}
			start += cubeVertices;

			for (math::matrix44_f32& instance : SphereInstances)
			{
				Program.SetUniform("model", instance);
				glDrawArrays(GL_TRIANGLES, start, sphereVertices);
			}
			start += sphereVertices;

			Program.SetUniform("model", math::identity4);
			glDrawArrays(GL_LINES, start, axesVertices);
		}

		Renderer.ImGuiContextPtr->RunFrame(std::move(imguiFrame));

		Renderer.RasterizerImpl->UpdateRenderBuffer();
	}
	void Graphics::ImGuiDebug()
	{
		ImGui::Text("Graphics");
		ImGui::ColorEdit3("BG Colour", reinterpret_cast<f32*>(&ClearColour));
		ImGui::Checkbox("Debug 2D", &Debug2D);
		ImGui::Checkbox("Debug 3D", &Debug3D);
	}
}

