#ifndef vulkanswapchain_h__
#define vulkanswapchain_h__

#include "graphics/api/ICommandBuffer.h"
#include "graphics/api/IImageView.h"
#include "graphics/api/ISwapChain.h"

#include <vector>

#include <vulkan/vulkan.h>

struct VulkanSwapChainSupportDetails
{
	VkSurfaceCapabilitiesKHR capabilities{};
	std::vector<VkSurfaceFormatKHR> formats{};
	std::vector<VkPresentModeKHR> presentModes{};
};

class VulkanSwapChain final : public ISwapChain
{
	public:
		explicit VulkanSwapChain(IGraphicsContext* aContext, uint8_t aFlightSize = 2);
		VulkanSwapChain(const VulkanSwapChain&) = delete;
		VulkanSwapChain(VulkanSwapChain&&) noexcept = delete;
		~VulkanSwapChain() override;

		VulkanSwapChain& operator=(const VulkanSwapChain&) = delete;
		VulkanSwapChain& operator=(VulkanSwapChain&&) noexcept = delete;

		void construct(const SwapChainCreateInfo& aInfo) override;
		void reconstruct(const SwapChainCreateInfo& aInfo) override;
		uint32_t getImageCount() override;

		VkSwapchainKHR getHandle() const;
		const std::vector<IImageView*>& getImageViews() const;

		IImageView* getDepthView() const;
		VkFormat getDepthFormat() const;

		void beginFrame();
		void submit(ICommandBuffer* aBuffer) const;
		void swap();

		EDataFormat getSwapchainFormat() const;
	private:
		void _createImageViews();
		void _createDepthResources();
		void _destroy();

		uint8_t mFlightSize = 2;
		uint32_t mCurrentImage = 0;
		uint32_t mImageCount = 0;
		std::vector<IImageView*> mImageViews;

		IImageView* mDepthView{};
		IImage* mDepthImage{};
		VmaAllocation mDepthMemory{};
		VkFormat mDepthFormat;

		VkSwapchainKHR mSwapchain{};
		VkSwapchainKHR mOldSwapchain{};
		IGraphicsContext* mContext;

		ICommandPool* mPool{};

		SwapChainCreateInfo mInfo{};
		EDataFormat mSwapchainImageFormat = EDataFormat::UNDEFINED;

		std::vector<VkSemaphore> mImageAvailable;
		std::vector<VkSemaphore> mRenderFinished;
		std::vector<VkFence> mFences;

		VkQueue mPresentQueue{};
		VkQueue mGraphicsQueue{};

		uint32_t mCurrentImageInChain = 0;
};

#endif // vulkanswapchain_h__
