#include <core/Log.h>
#include <core/PrimalCast.h>
#include <graphics/vk/VulkanCommandBuffer.h>
#include <graphics/vk/VulkanGraphicsContext.h>
#include <graphics/vk/VulkanImage.h>
#include <graphics/vk/VulkanImageView.h>
#include <graphics/vk/VulkanSwapChain.h>

namespace detail
{
	VkSurfaceFormatKHR SelectSwapSurfaceFormat(const VulkanSwapChainSupportDetails& aDetails)
	{
		if (aDetails.formats.size() == 1 && aDetails.formats[0].format == VK_FORMAT_UNDEFINED)
		{
			return { VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
		}
		for (const auto& format : aDetails.formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SNORM && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return format;
			}
		}
		return aDetails.formats[0];
	}

	VkPresentModeKHR SelectSwapPresentMode(const VulkanSwapChainSupportDetails& aDetails)
	{
		const VkPresentModeKHR best = VK_PRESENT_MODE_FIFO_KHR;

		for (const auto& mode : aDetails.presentModes)
		{
			if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return mode;
			}
			if (mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			{
				return mode;
			}
		}

		return best;
	}

	static DeviceQueueFamilyIndices sFindQueueFamilies(VkPhysicalDevice aDevice, const VkSurfaceKHR aSurface)
	{
		DeviceQueueFamilyIndices indices;
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(aDevice, &queueFamilyCount, nullptr);
		std::vector<VkQueueFamilyProperties> props;
		props.resize(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(aDevice, &queueFamilyCount, props.data());

		int32_t i = 0;
		for (const auto& queueFamily : props)
		{
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(aDevice, i, aSurface, &presentSupport);

			if (queueFamily.queueCount > 0 && presentSupport)
			{
				indices.presentFamily = i;
			}

			if (indices.graphicsFamily.has_value() && indices.presentFamily.has_value())
			{
				break;
			}

			i++;
		}

		return indices;
	}
}

VulkanSwapChain::VulkanSwapChain(IGraphicsContext* aContext, const uint8_t aFlightSize)
	: ISwapChain(aContext), mFlightSize(aFlightSize), mContext(aContext)
{
}

VulkanSwapChain::~VulkanSwapChain()
{
	_destroy();
}

void VulkanSwapChain::construct(const SwapChainCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	const VkDevice device = context->getDevice();
	const VkPhysicalDevice physicalDevice = context->getPhysicalDevice();
	const VkSurfaceKHR surface = context->getSurfaceHandle();

	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

	VkExtent2D extent;

	VulkanSwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);

	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
	if (formatCount)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
	if (presentModeCount)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
	}

	const VkSurfaceFormatKHR surfaceFormat = detail::SelectSwapSurfaceFormat(details);
	const VkPresentModeKHR presentMode = detail::SelectSwapPresentMode(details);

	if (details.capabilities.currentExtent.width != static_cast<uint32_t>(0xFFFFFFFF))
	{
		extent = details.capabilities.currentExtent;
	}
	else
	{
		VkExtent2D actual = { aInfo.width, aInfo.height };

		const auto minW = details.capabilities.maxImageExtent.width > actual.width ? actual.width : details.capabilities.maxImageExtent.width;
		actual.width = details.capabilities.minImageExtent.width > actual.width ? details.capabilities.minImageExtent.width : actual.width;

		const auto minH = details.capabilities.maxImageExtent.height > actual.height ? actual.height : details.capabilities.maxImageExtent.height;
		actual.height = details.capabilities.minImageExtent.height > actual.height ? details.capabilities.minImageExtent.height : actual.height;
		extent = actual;
	}

	uint32_t imageCount = details.capabilities.minImageCount + 1;
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount)
	{
		imageCount = details.capabilities.maxImageCount;
	}

	imageCount = imageCount > aInfo.maxImageCount ? aInfo.maxImageCount : imageCount;

	info.pNext = nullptr;
	info.surface = surface;
	info.minImageCount = imageCount;
	info.imageFormat = surfaceFormat.format;
	info.imageColorSpace = surfaceFormat.colorSpace;
	info.imageExtent = extent;
	info.imageArrayLayers = 1;
	info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	DeviceQueueFamilyIndices indices = detail::sFindQueueFamilies(physicalDevice, info.surface);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
	if (indices.graphicsFamily.value() != indices.presentFamily.value())
	{
		info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		info.queueFamilyIndexCount = 2;
		info.pQueueFamilyIndices = queueFamilyIndices;
	}
	else
	{
		info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		info.queueFamilyIndexCount = 0;
		info.pQueueFamilyIndices = nullptr;
	}

	info.preTransform = details.capabilities.currentTransform;
	info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	info.presentMode = presentMode;
	info.clipped = VK_TRUE;
	info.oldSwapchain = VK_NULL_HANDLE;

	const VkResult res = vkCreateSwapchainKHR(device, &info, nullptr, &mSwapchain);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan swapchain.");
		return;
	}
	PRIMAL_INTERNAL_INFO("Successfully created Vulkan swapchain.");

	mInfo = aInfo;

	mSwapchainImageFormat = static_cast<EDataFormat>(surfaceFormat.format);
	mImageViews.clear();

	mImageCount = imageCount;
	_createImageViews();
}

void VulkanSwapChain::reconstruct(const SwapChainCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

uint32_t VulkanSwapChain::getImageCount()
{
	return mImageCount;
}

VkSwapchainKHR VulkanSwapChain::getHandle() const
{
	return mSwapchain;
}

const std::vector<IImageView*>& VulkanSwapChain::getImageViews() const
{
	return mImageViews;
}

void VulkanSwapChain::submit(ICommandBuffer* aBuffer) const
{
	// TODO: Submit Buffers
	VulkanCommandBuffer* buffer = primal_cast<VulkanCommandBuffer*>(aBuffer);
}

void VulkanSwapChain::swap() const
{
	// TODO: Swap Buffers
}

EDataFormat VulkanSwapChain::getSwapchainFormat() const
{
	return mSwapchainImageFormat;
}

void VulkanSwapChain::_createImageViews()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	const VkDevice device = context->getDevice();

	std::vector<VkImage> swapChainImages;
	uint32_t imageCount = 0;
	vkGetSwapchainImagesKHR(device, mSwapchain, &imageCount, nullptr);
	swapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(device, mSwapchain, &imageCount, swapChainImages.data());

	for (const auto& image : swapChainImages)
	{
		VulkanImage* img = new VulkanImage(mContext);
		img->setHandle(image);
		VulkanImageView* imageView = new VulkanImageView(mContext);

		ImageViewCreateInfo createInfo = {};
		createInfo.image = img;
		createInfo.viewType = EImageViewType::IMAGE_VIEW_TYPE_2D;
		createInfo.format = mSwapchainImageFormat;
		createInfo.subresourceRange.layerCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.aspect = IMAGE_ASPECT_COLOR;

		imageView->construct(createInfo);
		mImageViews.emplace_back(imageView);
	}
}

void VulkanSwapChain::_destroy()
{
	for (IImageView* view : mImageViews)
	{
		delete view;
	}
	mImageViews.clear();

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroySwapchainKHR(context->getDevice(), mSwapchain, nullptr);
}
