#include "core/Log.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanSampler.h"

VulkanSampler::VulkanSampler(IGraphicsContext* aContext) 
	: ISampler(aContext)
{
	
}

VulkanSampler::~VulkanSampler()
{
	_destroy();
}

void VulkanSampler::construct(const SamplerCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	VkSamplerCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	info.flags = aInfo.flags;
	info.addressModeU = static_cast<VkSamplerAddressMode>(aInfo.addressModeU);
	info.addressModeV = static_cast<VkSamplerAddressMode>(aInfo.addressModeV);
	info.addressModeW = static_cast<VkSamplerAddressMode>(aInfo.addressModeW);
	info.anisotropyEnable = aInfo.anisotropyEnable;
	info.borderColor = static_cast<VkBorderColor>(aInfo.borderColor);
	info.compareEnable = aInfo.compareEnable;
	info.compareOp = static_cast<VkCompareOp>(aInfo.compareOp);
	info.magFilter = static_cast<VkFilter>(aInfo.magFilter);
	info.maxAnisotropy = aInfo.maxAnisotropy;
	info.maxLod = aInfo.maxLod;
	info.minFilter = static_cast<VkFilter>(aInfo.minFilter);
	info.minLod = aInfo.minLod;
	info.mipLodBias = aInfo.mipLodBias;
	info.mipmapMode = static_cast<VkSamplerMipmapMode>(aInfo.mipmapMode);
	info.unnormalizedCoordinates = aInfo.unnormalizedCoordinates;

	const VkResult res = vkCreateSampler(context->getDevice(), &info, nullptr, &mSampler);
	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan sampler.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan sampler.");
	}
}

VkSampler VulkanSampler::getHandle() const
{
	return mSampler;
}

void VulkanSampler::_destroy() const
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroySampler(context->getDevice(), mSampler, nullptr);
}
