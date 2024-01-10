#pragma once

#include "Debugger.h"

#if JM_DEBUG

#define JM_MATH_LOG(...) \
JM_LOG("Math", __VA_ARGS__)

#define JM_MATH_HALT(...) \
JM_HALT("Math", __VA_ARGS__)

#define JM_MATH_ASSERT(condition, ...) \
JM_ASSERT("Math", condition, __VA_ARGS__)

#define JM_MATH_VERIFY(condition, ...) \
JM_MATH_ASSERT(condition, __VA_ARGS__)

#else

#define JM_MATH_LOG(...)
#define JM_MATH_HALT(...)
#define JM_MATH_ASSERT(condition, ...)
#define JM_MATH_VERIFY(condition, ...) \
condition

#endif
