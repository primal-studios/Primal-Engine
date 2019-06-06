#ifndef iswapchain_h__
#define iswapchain_h__

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IImage.h"
#include "graphics/api/ICommandPool.h"

#include <cstdint>

struct SwapChainCreateInfo
{
	uint64_t surfaceHandle;
	uint32_t minImageCount;
	uint32_t maxImageCount;
	EDataFormat format;
	uint32_t width;
	uint32_t height;
	uint32_t layers;
	ImageUsageFlags usage;
};

class ISwapChain
{
	public:
		explicit ISwapChain(IGraphicsContext* aContext);
		ISwapChain(const ISwapChain&) = delete;
		ISwapChain(ISwapChain&&) noexcept = delete;
		virtual ~ISwapChain() = default;

		ISwapChain& operator=(const ISwapChain&) = delete;
		ISwapChain& operator=(ISwapChain&&) noexcept = delete;

		virtual void construct(const SwapChainCreateInfo&) = 0;
		virtual void reconstruct(const SwapChainCreateInfo&) = 0;

		virtual uint32_t getImageCount() = 0;
};

#endif // iswapchain_h__
