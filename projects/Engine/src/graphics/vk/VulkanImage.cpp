#include <graphics/vk/VulkanImage.h>
#include <graphics/vk/VulkanGraphicsContext.h>

#include <core/Log.h>

VulkanImage::VulkanImage(IGraphicsContext* aContext)
	: IImage(aContext), mContext(aContext)
{
	VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(aContext);
	mAllocator = ctx->getImageAllocator();
}

VulkanImage::~VulkanImage()
{
	vmaDestroyImage(mAllocator, mImage, mAllocation);
}

void VulkanImage::construct(const ImageCreateInfo& aInfo)
{
	using namespace std;

	VkImageCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.flags = aInfo.flags;
	createInfo.imageType = static_cast<VkImageType>(aInfo.dimension);
	createInfo.format = static_cast<VkFormat>(aInfo.format);
	createInfo.extent = { aInfo.width, aInfo.height, aInfo.depth };
	createInfo.mipLevels = aInfo.mipLayerCount;
	createInfo.arrayLayers = aInfo.arrayLayerCount;
	createInfo.samples = static_cast<VkSampleCountFlagBits>(aInfo.samples);
	createInfo.tiling = static_cast<VkImageTiling>(aInfo.tiling);
	createInfo.usage = aInfo.usage;
	createInfo.initialLayout = static_cast<VkImageLayout>(aInfo.layout);

	vector<uint32_t> queueFamilies;

	if (aInfo.contexts.size() <= 1)
	{
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	}
	else
	{
		createInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
	}

	for (auto context : aInfo.contexts)
	{
		VulkanGraphicsContext* ctx = reinterpret_cast<VulkanGraphicsContext*>(context);
		uint32_t q = ctx->getGraphicsQueueIndex();
		queueFamilies.push_back(q);
	}

	createInfo.pQueueFamilyIndices = queueFamilies.data();
	createInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilies.size());

	VmaAllocationCreateInfo allocInfo = {};
	allocInfo.requiredFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
	allocInfo.preferredFlags = VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT;
	allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

	const VkResult res = vmaCreateImage(mAllocator, &createInfo, &allocInfo, &mImage, &mAllocation, nullptr);

	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan image.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan image.");
	}
}

VkImage VulkanImage::getHandle() const
{
	return mImage;
}
