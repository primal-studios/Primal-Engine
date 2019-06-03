#ifndef vulkangraphicscontext_h__
#define vulkangraphicscontext_h__

#include "graphics/api/IGraphicsContext.h"

#include <optional>
#include <vector>

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

struct DeviceQueueFamilyIndices
{
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;
};

class VulkanGraphicsContext final : public IGraphicsContext
{
	public:
		explicit VulkanGraphicsContext(const GraphicsContextCreateInfo& aCreateInfo);
		VulkanGraphicsContext(const VulkanGraphicsContext&) = delete;
		VulkanGraphicsContext(VulkanGraphicsContext&&) noexcept = delete;
		~VulkanGraphicsContext() override;

		VulkanGraphicsContext& operator=(const VulkanGraphicsContext&) = delete;
		VulkanGraphicsContext& operator=(VulkanGraphicsContext&&) noexcept = delete;

		void idle() const override;

		VkSurfaceKHR getSurfaceHandle() const;
		VkDevice getDevice() const;
		VkPhysicalDevice getPhysicalDevice() const;

		uint32_t getGraphicsQueueIndex() const;
		uint32_t getPresentQueueIndex() const;

		VmaAllocator getImageAllocator() const;
		VmaAllocator getBufferAllocator() const;
	private:
		void _initializeVulkan();
		void _initializeDebugMessenger();
		bool _checkValidationLayerSupport() const;
		void _createPhysicalDevice();
		void _createLogicalDevice();
		std::vector<const char*> _getRequiredExtensions() const;

		std::vector<VkExtensionProperties> mProps;
		std::vector<const char*> mValidationLayers;
		std::vector<const char*> mDeviceExtensions;

		uint32_t mGraphicsQueueFamily = 0;
		uint32_t mPresentQueueFamily = 0;

		VkInstance mInstance{};
		VkDevice mDevice{};
		VkPhysicalDevice mPhysicalDevice{};
		VkSurfaceKHR mSurface{};
		VkDebugUtilsMessengerEXT mDebugMessenger{};
		GraphicsContextCreateInfo mCreateInfo;
		VmaAllocator mImageAllocator{};
		VmaAllocator mBufferAllocator{};
};

#endif // vulkangraphicscontext_h__
