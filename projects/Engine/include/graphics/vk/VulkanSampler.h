#ifndef vulkansampler_h__
#define vulkansampler_h__

#include "graphics/api/ISampler.h"

#include <vulkan/vulkan.h>

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
		void _destroy() const;

		VkSampler mSampler{};
};


#endif // vulkansampler_h__