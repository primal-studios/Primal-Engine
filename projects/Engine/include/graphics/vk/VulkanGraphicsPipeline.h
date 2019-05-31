#ifndef vulkangraphicspipeline_h__
#define vulkangraphicspipeline_h__

#include "graphics/api/IGraphicsPipeline.h"

class VulkanGraphicsPipeline final : public IGraphicsPipeline
{
	public:
		explicit VulkanGraphicsPipeline(IGraphicsContext* aContext);
		VulkanGraphicsPipeline(const VulkanGraphicsPipeline&) = delete;
		VulkanGraphicsPipeline(VulkanGraphicsPipeline&&) noexcept = delete;
		virtual ~VulkanGraphicsPipeline() override;

		VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline&) = delete;
		VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&&) noexcept = delete;

		void construct(const GraphicsPipelineCreateInfo& aInfo) override;
};

#endif // vulkangraphicspipeline_h__