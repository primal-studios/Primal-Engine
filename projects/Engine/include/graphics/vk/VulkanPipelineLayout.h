#ifndef vulkanpipelinelayout_h__
#define vulkanpipelinelayout_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IPipelineLayout.h"

class VulkanPipelineLayout final : public IPipelineLayout
{
	public:
		explicit VulkanPipelineLayout(IGraphicsContext* aContext);
		VulkanPipelineLayout(const VulkanPipelineLayout&) = delete;
		VulkanPipelineLayout(VulkanPipelineLayout&&) noexcept = delete;
		virtual ~VulkanPipelineLayout();

		VulkanPipelineLayout& operator=(const VulkanPipelineLayout&) = delete;
		VulkanPipelineLayout& operator=(VulkanPipelineLayout&&) noexcept = delete;

		void construct(const PipelineLayoutCreateInfo& aInfo) override;

		VkPipelineLayout getHandle() const;

	private:
		VkPipelineLayout mLayout{};
};

#endif // vulkanpipelinelayout_h__