#include "Debugger.h"

#include "PlatformCore.h"
#include "OS.h"

#include <iostream>
#include <sstream>
#include <cassert>
#include <algorithm>

namespace jm::Platform
{
	bool HasDebugger()
	{
		return IsDebuggerPresent();
	}
}

namespace jm
{
	void TrimDirectoryFromFilepath(std::string& string)
	{
		std::string substring;
		using Int = decltype(string.size());
		Int directoryTerminus = std::min(std::min(string.find_last_of('/'), string.size()), std::min(string.find_last_of('\\'), string.size()));
		for (auto idx = directoryTerminus + 1; idx < string.size(); ++idx)
		{
			substring.push_back(string.at(idx));
		}
		string = substring;
	}

	namespace Debugger
	{
		std::string MakeDebugString(Location location)
		{
			return MakeDebugString(location, "Don't panic!");
		}

		std::string MakeDebugString(Location location, cstring message)
		{
			constexpr DWORD size = 255;
			char rawModuleName[size];

			::std::ostringstream debugStream;

			if (GetModuleFileNameA(NULL, rawModuleName, size) < size)
			{
				std::string moduleName{ rawModuleName };
				std::string fileName{ location.fileName };
				TrimDirectoryFromFilepath(moduleName);
				TrimDirectoryFromFilepath(fileName);

				debugStream << "[ " << moduleName << " ][ " << __TIMESTAMP__ << " ]";
				debugStream << "[ " << fileName << " @ line " << location.lineNumber << " ]\n";
				debugStream << message << std::endl;
			}

			return debugStream.str();
		}

		std::string MakeDebugString(Location location, char* message)
		{
			return MakeDebugString(location, static_cast<cstring>(message));
		}

		void Log(cstring message)
		{
			OutputDebugStringA(message);
		}
	}
}