#pragma once

#include "PlatformCore.h"
#include "Modal.h"

namespace jm::Platform
{
	extern bool HasDebugger();
}

namespace jm::Debugger
{

	struct Location
	{
		decltype(__LINE__) lineNumber;
		cstring fileName;
		cstring projectName;
	};

	extern std::string MakeDebugString(Location location);

	extern std::string MakeDebugString(Location location, cstring message);

	extern std::string MakeDebugString(Location location, char* message);

	template <typename... Types>
	extern std::string MakeDebugString(Location location, cstring format, Types... values);

	template <typename Type>
	extern std::string MakeDebugString(Location location, Type value);

	template <typename... Types>
	std::string MakeDebugString(Location location, cstring format, Types... values)
	{
		std::string buffer(StringLength(format, values...), 0);
		std::snprintf(buffer.data(), buffer.size(), format, values...);
		return MakeDebugString(location, buffer.c_str());
	}

	template <typename Type>
	std::string MakeDebugString(Location location, Type value)
	{
		std::string valueString{"Value = " + std::to_string(value) };
		return MakeDebugString(location, valueString.c_str());
	}

	extern void Log(cstring message);
}

#define JM_LIKELY(condition) __assume(condition)

# define JM_BREAK if(::jm::Platform::HasDebugger()) { __debugbreak(); } std::exit(0)

#define __JM_DEBUG_STRING(project, ...) \
::jm::Debugger::MakeDebugString({__LINE__, __FILE__, project}, __VA_ARGS__).c_str()

#define __JM_USER_WANTS_DEBUG_BREAK(title, project, ...) \
::jm::Platform::ShouldInspectModal(project, title, __JM_DEBUG_STRING(project, __VA_ARGS__))

#define JM_LOG(project, ...) \
::jm::Debugger::Log(__JM_DEBUG_STRING(project, __VA_ARGS__))

#define __JM_PROMPT_DEBUG_BREAK(title, project, ...) \
JM_LOG(project, __VA_ARGS__); \
if (__JM_USER_WANTS_DEBUG_BREAK(title, project, __VA_ARGS__)) { JM_BREAK; }

#define __JM_MACRO_STATEMENT(expression) do {expression} while(false)

#define JM_HALT(project, ...) \
__JM_MACRO_STATEMENT(__JM_PROMPT_DEBUG_BREAK("Halted!", project, __VA_ARGS__))

#define JM_ASSERT(project, condition, ...) \
__JM_MACRO_STATEMENT(if (!(condition)) { __JM_PROMPT_DEBUG_BREAK("Assert failed!", project, __VA_ARGS__) })

#if defined DEBUG || defined _DEBUG || defined DBUG
#define JM_DEBUG 1
#else
#define JM_DEBUG 0
#endif

namespace jm::Platform
{
	constexpr bool IsDebug = bool(JM_DEBUG);
}