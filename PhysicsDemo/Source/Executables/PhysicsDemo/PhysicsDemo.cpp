#include "Visual/RenderingContext.h"
#include "Visual/DearImGui/ImGuiContext.h"

#include "Math/Camera.h"

#include "Platform/Tactual.h"
#include "Platform/WindowedApplication.h"

#include "Systems/Entity.h"
#include "Systems/Graphics.h"
#include "Systems/Components.h"
#include "Systems/Simulation.h"

#include "DearImGui/imgui.h"

#include "World.h"

namespace jm
{
	constexpr math::vector2<iSize> screenSize = { 1600, 900 };
	constexpr math::vector2_f32 screenSizeFloat = { f32(screenSize.x), f32(screenSize.y) };

	struct LoopController final
	{
		static constexpr uSize FixedTick_Frequency = 120; //Hz
		static constexpr f64 FixedTick_Period = 1.0 / FixedTick_Frequency; //s

		static constexpr uSize FPSUpdate_Frequency = 4; //Hz
		static constexpr f64 FPSUpdate_Period = 1.0 / FPSUpdate_Frequency; //s

		LoopController()
		{
			Timer.Initialize();
		}

		void Step()
		{
			Timer.Update();
			FPSCounter++;

			const f64 dt = Timer.GetElapsedTime();
			FPSUpdateAccumulator += dt;
			if (FPSUpdateAccumulator > FPSUpdate_Period)
			{
				FPSUpdateAccumulator -= FPSUpdate_Period;
				FPS = FPSUpdate_Frequency * FPSCounter;
				FPSCounter = 0;
			}

			SimUpdateAccumulator += dt;
			if (SimUpdateAccumulator > FixedTick_Period)
			{
				SimUpdateAccumulator -= FixedTick_Period;
				ShouldTick = true;
			}
			else
			{
				ShouldTick = false;
			}

		}

		f32 GetLoopDeltaTime()
		{
			return static_cast<f32>(Timer.GetElapsedTime());
		}

		uSize GetFPS()
		{
			return FPS;
		}

		bool ShouldTickThisFrame()
		{
			return ShouldTick;
		}

	private:

		Platform::Timer Timer;
		f64 SimUpdateAccumulator = 0.0;
		f64 FPSUpdateAccumulator = 0.0;
		uSize FPSCounter = 0;
		uSize FPS = 60;
		bool ShouldTick = false;
	};

	math::camera3<f32> Make3DCamera(f32 distanceFromOrigin, f32 yFOV, f32 aspectRatio)
	{
		math::vector3_f32 lookFrom = distanceFromOrigin * math::vector3_f32{ 1.0f };
		math::vector3_f32 lookAt = { 0.0f, 0.0f, 0.0f };
		return math::camera3<f32>(lookFrom, lookAt, yFOV, aspectRatio);
	}

	math::vector2_f32 GetWorldFromScreenPosition(math::camera3<f32> const& camera, Platform::ScreenPosition screen)
	{
		math::vector2_f32 screenPositionFloat = { screen.X, screen.Y };
		math::vector2_f32 screenNormed = 2.0f * ((screenPositionFloat / screenSizeFloat) - 0.5f);
		screenNormed.y = -screenNormed.y;
		return camera.get_screen_to_world(screenNormed);
	}

	struct PhysicsDemo : Platform::WindowedApplication
	{
		PhysicsDemo(const Platform::RuntimeContext& context)
			: Platform::WindowedApplication(context, { "3D", { 50 , 50 }, { screenSize.x, screenSize.y } })
			, Camera(Make3DCamera(10.0f, 45.0f, window->GetArea().GetAspectRatio()))
			, registry()
			, InputSystem()
			, GraphicsSystem(*window, registry)
		{

		}

		virtual ~PhysicsDemo() override = default;

		virtual void OnStartLoop() override
		{
			AddMessageHandler(GraphicsSystem.GetMessageHandler());
			AddMessageHandler(InputSystem.GetMessageHandler());
			
			CreateWorld();
		}

		virtual void RunLoop() override
		{
			Controller.Step();

			InputUpdate();

			if (Simulating && Controller.ShouldTickThisFrame())
			{
				SimulationUpdate();
			}

			uSize fps = Controller.GetFPS();
			GraphicsSystem.Draw3D(Camera, [this, fps]()
				{
					ImGui::Begin("Data");
					ImGui::Text("FPS = %d", fps);

					ImGui::Text("Simulation");
					if (ImGui::Button("Play"))
					{
						Simulating = true;
					}
					ImGui::SameLine();
					if (ImGui::Button("Pause"))
					{
						Simulating = false;
					}
					ImGui::SameLine();
					if (ImGui::Button("Reset"))
					{
						DestroyWorld();
						CreateWorld();
					}

					GraphicsSystem.ImGuiDebug();

					ImGui::Text("Entities");
					ImGui::Text("Count = %d", registry.storage<entity_id>().in_use());
					/*if (SelectedEntity.has_value())
					{
						ImGui::Text("Selected = %d", get_entity_id_raw(SelectedEntity.value().Entity));
					}
					else
					{
						ImGui::Text("Selected = null");
					}*/
					ImGui::End();
				});

			Running = !InputSystem.GetKeyboard().EscPressed;
		}

		virtual void OnStopLoop() override
		{
			DestroyWorld();
			
			RemoveMessageHandler(InputSystem.GetMessageHandler());
			RemoveMessageHandler(GraphicsSystem.GetMessageHandler());
		}

		void HandleException(const std::exception& applicationException)
		{
			JM_HALT("Application", applicationException.what());
		}

		void CreateWorld()
		{
			CreateBasicWorld(registry);
		}

		void DestroyWorld()
		{
			registry.clear();
		}

		void InputUpdate()
		{
			const bool shiftPressed = InputSystem.GetKeyboard().ShiftPressed;
			const float cameraTranslatSpeed = (shiftPressed ? 3.0f : 1.5f) * float(Controller.GetLoopDeltaTime());
			const float cameraRotateSpeed = float(Controller.GetLoopDeltaTime());

			if (InputSystem.GetKeyboard().WPressed)
			{
				Camera.translate(cameraTranslatSpeed * Camera.get_forward());
			}
			if (InputSystem.GetKeyboard().SPressed)
			{
				Camera.translate(cameraTranslatSpeed * Camera.get_back());
			}
			if (InputSystem.GetKeyboard().APressed)
			{
				Camera.translate(cameraTranslatSpeed * Camera.get_left());
			}
			if (InputSystem.GetKeyboard().DPressed)
			{
				Camera.translate(cameraTranslatSpeed * Camera.get_right());
			}
			if (InputSystem.GetKeyboard().UpPressed)
			{
				Camera.rotate(0.0f, cameraRotateSpeed);
			}
			if (InputSystem.GetKeyboard().DownPressed)
			{
				Camera.rotate(0.0f, -cameraRotateSpeed);
			}
			if (InputSystem.GetKeyboard().RightPressed)
			{
				Camera.rotate(cameraRotateSpeed, 0.0f);
			}
			if (InputSystem.GetKeyboard().LeftPressed)
			{
				Camera.rotate(-cameraRotateSpeed, 0.0f);
			}

			InputSystem.Update();
		}

		void SimulationUpdate()
		{
			integrate(registry, static_cast<f32>(LoopController::FixedTick_Period));
		}

		entity_registry registry;
		LoopController Controller;
		bool Simulating = false;

		math::camera3<f32> Camera;
		

		Tactual::System InputSystem;
		System::Graphics GraphicsSystem;
	};

}


JM_APPLICATION_MAIN("Physics Demo", jm::PhysicsDemo)
