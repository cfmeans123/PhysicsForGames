#include "Application.h"
#include "PlatformDebug.h"

#include <shellapi.h>

namespace jm::Platform
{
	//from http://alter.org.ua/en/docs/win/args/
	PCHAR* CommandLineToArgvA(PCHAR CmdLine, int* _argc)
	{
		PCHAR* argv;
		PCHAR  _argv;
		std::size_t len;
		ULONG   argc;
		std::size_t i, j;

		BOOLEAN  in_QM;
		BOOLEAN  in_TEXT;
		BOOLEAN  in_SPACE;

		len = strlen(CmdLine);
		i = ((len + 2) / 2) * sizeof(PVOID) + sizeof(PVOID);

		argv = (PCHAR*)GlobalAlloc(GMEM_FIXED,
			i + (len + 2) * sizeof(CHAR));

		_argv = (PCHAR)(((PUCHAR)argv) + i);

		argc = 0;
		argv[argc] = _argv;
		in_QM = FALSE;
		in_TEXT = FALSE;
		in_SPACE = TRUE;
		i = 0;
		j = 0;

		while (CHAR a = CmdLine[i]) {
			if (in_QM) {
				if (a == '\"') {
					in_QM = FALSE;
				}
				else {
					_argv[j] = a;
					j++;
				}
			}
			else {
				switch (a) {
				case '\"':
					in_QM = TRUE;
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					in_SPACE = FALSE;
					break;
				case ' ':
				case '\t':
				case '\n':
				case '\r':
					if (in_TEXT) {
						_argv[j] = '\0';
						j++;
					}
					in_TEXT = FALSE;
					in_SPACE = TRUE;
					break;
				default:
					in_TEXT = TRUE;
					if (in_SPACE) {
						argv[argc] = _argv + j;
						argc++;
					}
					_argv[j] = a;
					j++;
					in_SPACE = FALSE;
					break;
				}
			}
			i++;
		}
		_argv[j] = '\0';
		argv[argc] = NULL;

		(*_argc) = argc;
		return argv;
	}


	RuntimeContext::RuntimeContext(cstring name, HINSTANCE hInstance, HINSTANCE, PSTR, INT)
		: ApplicationName(name)
		, Instance(hInstance)
	{
		INT argc;
		LPSTR* argv = CommandLineToArgvA(GetCommandLineA(), &argc);

		for (int i = 0; i < argc; ++i)
		{
			CommandLineArguments.push_back(argv[i]);
		}

		JM_PLATFORM_ASSERT(CommandLineArguments.size() > 0);
	}

	RuntimeContext::RuntimeContext(cstring name, int argc, char* argv[])
		: ApplicationName(name)
		, Instance(GetModuleHandle(NULL))
	{
		for (int i = 0; i < argc; ++i)
		{
			CommandLineArguments.push_back(argv[i]);
		}
	}
}
