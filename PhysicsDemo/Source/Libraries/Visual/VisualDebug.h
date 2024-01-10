#pragma once

#include "Platform/Debugger.h"

#if JM_DEBUG

#define JM_VISUAL_LOG(...) \
JM_LOG("Visual", __VA_ARGS__)

#define JM_VISUAL_HALT(...) \
JM_HALT("Visual", __VA_ARGS__)

#define JM_VISUAL_ASSERT(condition, ...) \
JM_ASSERT("Visual", condition, __VA_ARGS__)

#define JM_VISUAL_VERIFY(condition, ...) \
JM_VISUAL_ASSERT(condition, __VA_ARGS__)

#else

#define JM_VISUAL_LOG(...)
#define JM_VISUAL_HALT(...)
#define JM_VISUAL_ASSERT(condition, ...)
#define JM_VISUAL_VERIFY(condition, ...) \
condition

#endif
