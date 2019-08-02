#include "core/Log.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanCommandBuffer.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanImage.h"
#include "graphics/vk/VulkanImageView.h"
#include "graphics/vk/VulkanSwapChain.h"

namespace detail
{
	static VkSurfaceFormatKHR sSelectSwapSurfaceFormat(const VulkanSwapChainSupportDetails& aDetails)
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

	static VkPresentModeKHR sSelectSwapPresentMode(const VulkanSwapChainSupportDetails& aDetails)
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

	static DeviceQueueFamilyIndices sFindQueueFamilies(const VkPhysicalDevice aDevice, const VkSurfaceKHR aSurface)
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

	static VkFormat sFindSupportedFormat(const VkPhysicalDevice aDevice, const std::vector<VkFormat>& aCandidates, VkImageTiling aTiling, VkFormatFeatureFlags aFeatures)
	{
		for(VkFormat format : aCandidates)
		{
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(aDevice, format, &props);

			if(aTiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & aFeatures) == aFeatures)
			{
				return format;
			}
			else if(aTiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & aFeatures) == aFeatures)
			{
				return format;
			}
		}

		PRIMAL_INTERNAL_CRITICAL("Failed to find supported Vulkan format.");

		return VK_FORMAT_UNDEFINED;
	}

	static VkFormat sFindSupportedDepthFormat(const VkPhysicalDevice aDevice)
	{
		return sFindSupportedFormat(aDevice, { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
	}
}

VulkanSwapChain::VulkanSwapChain(IGraphicsContext* aContext, const uint8_t aFlightSize)
	: ISwapChain(aContext), mFlightSize(aFlightSize), mDepthFormat(), mContext(aContext)
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

	mPool = context->getCommandPool();

	VkSwapchainCreateInfoKHR info = {};
	info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	info.oldSwapchain = mOldSwapchain;

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

	const VkSurfaceFormatKHR surfaceFormat = detail::sSelectSwapSurfaceFormat(details);
	const VkPresentModeKHR presentMode = detail::sSelectSwapPresentMode(details);

	if (details.capabilities.currentExtent.width != static_cast<uint32_t>(0xFFFFFFFF))
	{
		extent = details.capabilities.currentExtent;
	}
	else
	{
		int width, height;
		glfwGetFramebufferSize(reinterpret_cast<GLFWwindow*>(aInfo.windowHandle), &width, &height);
		VkExtent2D actual = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

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
	if (aInfo.oldSwapchain)
	{
		info.oldSwapchain = primal_cast<VulkanSwapChain*>(aInfo.oldSwapchain)->getHandle();
	}
	else
	{
		info.oldSwapchain = VK_NULL_HANDLE;
	}

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
	_createDepthResources();

	mImageAvailable.resize(mFlightSize);
	mRenderFinished.resize(mFlightSize);
	mFences.resize(mFlightSize);

	VkSemaphoreCreateInfo semInfo = {};
	semInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (uint32_t i = 0; i < mFlightSize; i++)
	{
		if (vkCreateSemaphore(context->getDevice(), &semInfo, nullptr, &mImageAvailable[i]) != VK_SUCCESS ||
			vkCreateSemaphore(context->getDevice(), &semInfo, nullptr, &mRenderFinished[i]) != VK_SUCCESS ||
			vkCreateFence(context->getDevice(), &fenceInfo, nullptr, &mFences[i]) != VK_SUCCESS)
		{
			PRIMAL_INTERNAL_CRITICAL("Failed to create synchronization primitives for frame {0}.", i);
		}
	}

	vkGetDeviceQueue(context->getDevice(), context->getPresentQueueIndex(), 0, &mPresentQueue);
	vkGetDeviceQueue(context->getDevice(), context->getGraphicsQueueIndex(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(context->getDevice(), context->getTransferQueueIndex(), 0, &mTransferQueue);

	vkWaitForFences(context->getDevice(), 1, &mFences[mCurrentImage], VK_TRUE, 0xFFFFFFFFFFFFFFFF);
	vkResetFences(context->getDevice(), 1, &mFences[mCurrentImage]);
}

void VulkanSwapChain::reconstruct(const SwapChainCreateInfo& aInfo)
{
	construct(aInfo);
	destroy();
}

void VulkanSwapChain::destroy()
{
	_destroy();
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

IImageView* VulkanSwapChain::getDepthView() const
{
	return mDepthView;
}

VkFormat VulkanSwapChain::getDepthFormat() const
{
	return mDepthFormat;
}

void VulkanSwapChain::beginFrame()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	const VkDevice device = context->getDevice();

	const auto result = vkAcquireNextImageKHR(device, mSwapchain, std::numeric_limits<uint64_t>::max(), mImageAvailable[mCurrentImage], nullptr, &mCurrentImageInChain);
}

void VulkanSwapChain::submitToTransferQueue(ICommandBuffer* aBuffer, bool aWaitForFinish) const
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	const VkDevice device = context->getDevice();

	VkFence fence = VK_NULL_HANDLE;
	if (aWaitForFinish) 
	{
		VkFenceCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		createInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		
		vkCreateFence(device, &createInfo, nullptr, &fence);
	}

	VulkanCommandBuffer* buffer = primal_cast<VulkanCommandBuffer*>(aBuffer);

	auto& toWaitOn = buffer->getSemaphoresToWaitOn();
	auto& toSignal = buffer->getSemaphoresToSignal();

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	PipelineStageFlags flags[] = { PIPELINE_STAGE_DRAW_INDIRECT };
	submitInfo.pWaitDstStageMask = flags;

	if (!toWaitOn.empty()) 
	{
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(toWaitOn.size());
		submitInfo.pWaitSemaphores = toWaitOn.data();
	}
	else
	{
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &mImageAvailable[mCurrentImage];
	}
	if (!toSignal.empty())
	{
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(toSignal.size());
		submitInfo.pSignalSemaphores = toSignal.data();
	}

	VkCommandBuffer buf = buffer->getHandle();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buf;
	
	vkQueueSubmit(mTransferQueue, 1, &submitInfo, fence);

	if (aWaitForFinish)
	{
		vkDestroyFence(device, fence, nullptr);
	}
}

void VulkanSwapChain::submit(ICommandBuffer* aBuffer, bool aIsLastSubmission) const
{
	VulkanCommandBuffer* buffer = primal_cast<VulkanCommandBuffer*>(aBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.pWaitDstStageMask = waitStages;

	auto& toWaitOn = buffer->getSemaphoresToWaitOn();
	auto& toSignal = buffer->getSemaphoresToSignal();
	
	if (toWaitOn.empty())
	{
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = &mImageAvailable[mCurrentImage];
	}
	else
	{
		submitInfo.waitSemaphoreCount = static_cast<uint32_t>(toWaitOn.size());
		submitInfo.pWaitSemaphores = toWaitOn.data();
	}

	if (toSignal.empty())
	{
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = &mRenderFinished[mCurrentImage];
	}
	else
	{
		submitInfo.signalSemaphoreCount = static_cast<uint32_t>(toSignal.size());
		submitInfo.pSignalSemaphores = toSignal.data();
	}

	VkCommandBuffer buf = buffer->getHandle();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &buf;

	vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, aIsLastSubmission ? mFences[mCurrentImage] : VK_NULL_HANDLE);
}

bool VulkanSwapChain::swap()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &mRenderFinished[mCurrentImage];

	VkSwapchainKHR swapChains[] = { mSwapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;

	presentInfo.pImageIndices = &mCurrentImageInChain;

	const VkResult res = vkQueuePresentKHR(mPresentQueue, &presentInfo);
	PRIMAL_ASSERT(res == VK_SUCCESS, "Failed to swap Vulkan swap chain");

	mCurrentImage = (mCurrentImage + 1) % mFlightSize;

	vkWaitForFences(context->getDevice(), 1, &mFences[mCurrentImage], VK_TRUE, 0xFFFFFFFFFFFFFFFF);
	vkResetFences(context->getDevice(), 1, &mFences[mCurrentImage]);

	return res == VK_SUCCESS;
}

EDataFormat VulkanSwapChain::getSwapchainFormat() const
{
	return mSwapchainImageFormat;
}

VkQueue VulkanSwapChain::getGraphicsQueue() const
{
	return mGraphicsQueue;
}

VkQueue VulkanSwapChain::getPresentQueue() const
{
	return mPresentQueue;
}

VkQueue VulkanSwapChain::getTransferQueue() const
{
	return mTransferQueue;
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

void VulkanSwapChain::_createDepthResources()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);

	mDepthFormat = detail::sFindSupportedDepthFormat(context->getPhysicalDevice());

	mDepthImage = new VulkanImage(mContext);
	ImageCreateInfo createInfo = {};
	createInfo.flags = 0;
	createInfo.dimension = EImageDimension::IMAGE_2D;
	createInfo.format = static_cast<EDataFormat>(mDepthFormat);
	createInfo.width = mInfo.width;
	createInfo.height = mInfo.height;
	createInfo.depth = 1;
	createInfo.baseMipLevel = 0;
	createInfo.mipLayerCount = 1;
	createInfo.baseArrayLayer = 0;
	createInfo.levelCount = 1;
	createInfo.layerCount = 1;
	createInfo.samples = IMAGE_SAMPLE_1;
	createInfo.tiling = EImageTiling::IMAGE_TILING_OPTIMAL;
	createInfo.usage = IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	createInfo.layout = IMAGE_LAYOUT_UNDEFINED;
	createInfo.contexts = { mContext };
	createInfo.memoryRequiredFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	createInfo.imageAspect = IMAGE_ASPECT_DEPTH;

	mDepthImage->construct(createInfo);

	mDepthView = new VulkanImageView(mContext);
	mDepthView->construct({ mDepthImage, static_cast<EDataFormat>(mDepthFormat), EImageViewType::IMAGE_VIEW_TYPE_2D, {EImageAspectFlagBits::IMAGE_ASPECT_DEPTH, 0, 1, 0, 1} });

	VulkanImage* vImage = primal_cast<VulkanImage*>(mDepthImage);

	vImage->transitionToLayout(createInfo, static_cast<EDataFormat>(mDepthFormat), IMAGE_LAYOUT_UNDEFINED, IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void VulkanSwapChain::_destroy()
{
	VulkanGraphicsContext* context = reinterpret_cast<VulkanGraphicsContext*>(mContext);
	vkDeviceWaitIdle(context->getDevice());

	for (IImageView* view : mImageViews)
	{
		delete view;
	}

	mImageViews.clear();

	delete mDepthView;
	delete mDepthImage;

	vkDeviceWaitIdle(context->getDevice());
	vkDestroySwapchainKHR(context->getDevice(), mSwapchain, nullptr);

	for (auto sem : mImageAvailable)
	{
		vkDestroySemaphore(context->getDevice(), sem, nullptr);
	}

	for (auto sem : mRenderFinished)
	{
		vkDestroySemaphore(context->getDevice(), sem, nullptr);
	}

	for (auto fence : mFences)
	{
		vkDestroyFence(context->getDevice(), fence, nullptr);
	}

	mFences.clear();
	mImageAvailable.clear();
	mRenderFinished.clear();
}
