#pragma once

#include "Debugger.h"

#if JM_DEBUG

#define JM_PLATFORM_LOG(...) \
JM_LOG("Platform", __VA_ARGS__)

#define JM_PLATFORM_HALT(...) \
JM_HALT("Platform", __VA_ARGS__)

#define JM_PLATFORM_ASSERT(condition, ...) \
JM_ASSERT("Platform", condition, __VA_ARGS__)

#define JM_PLATFORM_VERIFY(condition, ...) \
JM_PLATFORM_ASSERT(condition, __VA_ARGS__)

#else

#define JM_PLATFORM_LOG(...)
#define JM_PLATFORM_HALT(...)
#define JM_PLATFORM_ASSERT(condition, ...)
#define JM_PLATFORM_VERIFY(condition, ...) \
condition

#endif
