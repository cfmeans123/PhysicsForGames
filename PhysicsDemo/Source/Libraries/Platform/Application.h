#pragma once

#include "PlatformCore.h"
#include "OS.h"

#include <vector>

namespace jm::Platform
{
	struct RuntimeContext
	{
		RuntimeContext(cstring name, HINSTANCE hInstance, HINSTANCE hPrevInstance, PSTR lpCmdLine, INT nCmdShow);

		RuntimeContext(cstring name, int argc, char* argv[]);

		std::string ApplicationName;

		HINSTANCE Instance;

		std::vector<std::string> CommandLineArguments;
	};

	template <typename TApplication>
	int RunMain(const RuntimeContext& context)
	{
		TApplication app(context);
		try
		{
			return app.Run();
		}
		catch (std::exception const& exception)
		{
			app.HandleException(exception);
		}

		return 0;
	}
}

#define JM_APPLICATION_MAIN(appName, appType) \
int main(int argc, char* argv[]) \
{ \
	auto context = jm::Platform::RuntimeContext(appName, argc, argv); \
	return jm::Platform::RunMain<appType>(context); \
}

#define JM_APPLICATION_SYSTEM_MAIN(appName, appType) \
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) \
{ \
	auto context = jm::Platform::RuntimeContext(hInstance, hPrevInstance, lpCmdLine, nShowCmd); \
	return jm::Platform::RunMain<appType>(appName, context); \
}
