#include "Window.h"

#include "Application.h"

namespace jm::Platform
{
	Window::Window(const RuntimeContext& runtimeContext, WindowParameters parameters)
		: Runtime(runtimeContext)
		, Name(runtimeContext.ApplicationName + " - " + parameters.Name)
		, Origin(parameters.Origin)
		, Area(parameters.Area)
	{
	}
}
