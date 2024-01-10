#pragma once

#include "Singleton.h"
#include "WindowedApplication.h"

#include <memory>

namespace jm::Tactual
{
	class System;

	struct InputHandler : Platform::MessageHandler
	{
		InputHandler(System& state);

		virtual ~InputHandler() {}

		System& stateSystem;

		virtual bool ProcessSystemMessage(MSG& systemInput) override;
	};

	struct KeyboardState
	{
		bool EscPressed = false;
		bool ShiftPressed = false;
		bool CtrlPressed = false;
		bool SpacePressed = false;

		bool APressed = false;
		bool DPressed = false;
		bool FPressed = false;
		bool RPressed = false;
		bool SPressed = false;
		bool WPressed = false;

		bool UpPressed = false;
		bool DownPressed = false;
		bool LeftPressed = false;
		bool RightPressed = false;
	};

	struct MouseState
	{
		bool LeftPressed = false;
		bool RightPressed = false;

		bool LeftPressedPrevious = false;
		bool RightPressedPrevious = false;

		Platform::ScreenPosition relative;
		Platform::ScreenPosition absolute;

		bool MouseClickDown() { return LeftPressed && !LeftPressedPrevious; }

		bool MouseClickUp() { return !LeftPressed && LeftPressedPrevious; }
	};

	class System : public singleton<System>
	{
		InputHandler MessageHandler;
		KeyboardState Keyboard;
		MouseState Mouse;

	public:

		System();
		~System();

		void Update()
		{
			Mouse.LeftPressedPrevious = Mouse.LeftPressed;
			Mouse.RightPressedPrevious = Mouse.RightPressed;
		}

		Platform::MessageHandler* GetMessageHandler()
		{
			return &MessageHandler;
		}

		KeyboardState& GetKeyboard()
		{
			return Keyboard;
		}

		MouseState& GetMouse()
		{
			return Mouse;
		}
	};
}