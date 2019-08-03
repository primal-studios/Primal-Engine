#ifndef vulkangraphicspipeline_h__
#define vulkangraphicspipeline_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IGraphicsPipeline.h"
#include "VulkanPipelineLayout.h"

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
		void reconstruct(const GraphicsPipelineCreateInfo& aInfo) override;
		void destroy() override;

		GraphicsPipelineCreateInfo& getCreateInfo() override;
		VkPipeline getHandle() const;
		VulkanPipelineLayout* getLayout() const;

	private:
		void _destroy() const;

		VkPipeline mPipeline{};
};

#endif // vulkangraphicspipeline_h__