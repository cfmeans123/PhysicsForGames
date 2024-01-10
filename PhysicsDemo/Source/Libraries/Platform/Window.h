#pragma once

#include "PlatformCore.h"

namespace jm::Platform
{
	struct RuntimeContext;

	struct WindowParameters
	{
		cstring Name;
		ScreenPosition Origin;
		PixelArea Area;
	};

	class Window
	{
	public:
		Window(const RuntimeContext& runtimeContext, WindowParameters parameters);

		virtual ~Window() = default;

		bool WasClosed() const { return Closed; };

		cstring GetName() const { return Name.c_str(); }

		PixelArea GetArea() const { return Area; }

	protected:

		void Open() { Closed = false; }
		void Close() { Closed = true; }

	private:

		const RuntimeContext& Runtime;

		std::string Name;
		ScreenPosition Origin;
		PixelArea Area;
		bool Closed = true;
	};
}
