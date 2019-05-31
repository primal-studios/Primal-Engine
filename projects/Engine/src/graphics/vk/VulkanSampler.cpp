#include "graphics/vk/VulkanSampler.h"

VulkanSampler::VulkanSampler(IGraphicsContext* aContext) : ISampler(aContext)
{
	mSampler = nullptr;
}

VulkanSampler::~VulkanSampler()
{
	
}

void VulkanSampler::construct(const SamplerCreateInfo& aInfo)
{
	
}

VkSampler VulkanSampler::getHandle() const
{
	return mSampler;
}

