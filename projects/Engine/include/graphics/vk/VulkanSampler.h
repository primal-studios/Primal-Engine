#ifndef vulkansampler_h__
#define vulkansampler_h__

#include <vulkan/vulkan.h>

#include "graphics/api/ISampler.h"

class VulkanSampler final : public ISampler
{
	public:
		explicit VulkanSampler(IGraphicsContext* aContext);
		VulkanSampler(const VulkanSampler&) = delete;
		VulkanSampler(VulkanSampler&&) noexcept = delete;
		virtual ~VulkanSampler() override;

		VulkanSampler& operator=(const VulkanSampler&) = delete;
		VulkanSampler& operator=(VulkanSampler&&) noexcept = delete;

		void construct(const SamplerCreateInfo& aInfo) override;

		VkSampler getHandle() const;

	private:
		VkSampler mSampler;
};


#endif // vulkansampler_h__