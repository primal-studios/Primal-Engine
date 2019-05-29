#ifndef vulkanswapchain_h__
#define vulkanswapchain_h__

#include <graphics/api/ICommandBuffer.h>
#include <graphics/api/IImageView.h>
#include <graphics/api/ISwapChain.h>

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

		VkSwapchainKHR getHandle() const;
		const std::vector<IImageView*>& getImageViews() const;

		void submit(ICommandBuffer* aBuffer) const;
		void swap() const;
	private:
		void _createImageViews();
		void _destroy();

		uint8_t mFlightSize;
		uint32_t mCurrentImage = 0;
		std::vector<IImageView*> mImageViews;

		VkSwapchainKHR mSwapchain{};
		IGraphicsContext* mContext;

		SwapChainCreateInfo mInfo{};
		EDataFormat mSwapchainImageFormat = EDataFormat::UNDEFINED;
};

#endif // vulkanswapchain_h__
