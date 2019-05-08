#ifndef vma_h__
#define vma_h__

#if defined (__WIN32)

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR

#pragma warning (push, 4)
#pragma warning (disable: 4127)
#pragma warning (disable: 4100)
#pragma warning (disable: 4189)

#include "vm_mem_alloc.h"

#pragma warning (pop)

#endif // defined (_WIN32)

#if defined (__clang__)

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-compare"

#endif // defined (__clang__)

#include <vulkan/vulkan.h>
#include "vk_mem_alloc.h"

#if defined (__clang__)

#pragma clang diagonstic pop

#endif // defined (__clang__)

#endif // vma_h__
