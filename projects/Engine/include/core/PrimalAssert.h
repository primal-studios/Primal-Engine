#ifndef primalassert_h__
#define primalassert_h__

#include "core/Log.h"

#if defined(PRIMAL_PLATFORM_WINDOWS)
#define DEBUG_BREAK __debugbreak()
#elif defined(PRIMAL_PLATFORM_LINUX)
#include <signal.h>
#define DEBUG_BREAK raise(SIGTRAP)
#include <signal.h>
#elif defined(PRIMAL_PLATFORM_MAC)
#define DEBUG_BREAK raise(SIGTRAP)
#endif

#ifdef PRIMAL_ENABLE_ASSERTS
#define PRIMAL_ASSERT(x, ...) { if(!(x)) { PRIMAL_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; } }
#define PRIMAL_INTERNAL_ASSERT(x, ...) { if(!(x)) { PRIMAL_INTERNAL_ERROR("Assertion Failed: {0}", __VA_ARGS__); DEBUG_BREAK; } }
#else
#define PRIMAL_ASSERT(x, ...)
#define PRIMAL_INTERNAL_ASSERT(x, ...)
#endif

#if defined (_DEBUG)
#define DEBUG_ONLY_BLOCK(block) block
#define RELEASE_ONLY_BLOCK(block)
#define DEBUG_RELEASE_TOGGLE_BLOCK(dbg, rel) dbg
#else
#define DEBUG_ONLY_BLOCK(block)
#define RELEASE_ONLY_BLOCK(block) block
#define DEBUG_RELEASE_TOGGLE_BLOCK(dbg, rel) rel
#endif 

#endif // primalassert_h__