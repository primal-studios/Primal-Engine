#ifndef vulkanshaderstage_h__
#define vulkanshaderstage_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IShaderStage.h"

class VulkanShaderStage final : public IShaderStage
{
	public:
		explicit VulkanShaderStage(IGraphicsContext* aContext);
		VulkanShaderStage(const VulkanShaderStage&) = delete;
		VulkanShaderStage(VulkanShaderStage&&) noexcept = delete;
		virtual ~VulkanShaderStage() = default;

		VulkanShaderStage& operator=(const VulkanShaderStage&) = delete;
		VulkanShaderStage& operator=(VulkanShaderStage&&) noexcept = delete;

		void construct(const ShaderStageCreateInfo& aInfo) override;

		VkPipelineShaderStageCreateInfo getStage() const;

	private:
		VkPipelineShaderStageCreateInfo mStage;
};

#endif // vulkanshaderstage_h__