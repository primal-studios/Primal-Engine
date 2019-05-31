#ifndef vulkanshadermodule_h__
#define vulkanshadermodule_h__

#include <vulkan/vulkan.h>

#include "graphics/api/IShaderModule.h"

class VulkanShaderModule final : public IShaderModule
{
	public:
		explicit VulkanShaderModule(IGraphicsContext* aContext);
		VulkanShaderModule(const VulkanShaderModule&) = delete;
		VulkanShaderModule(VulkanShaderModule&&) noexcept = delete;
		virtual ~VulkanShaderModule() override;

		VulkanShaderModule& operator=(const VulkanShaderModule&) = delete;
		VulkanShaderModule& operator=(VulkanShaderModule&&) noexcept = delete;

		void construct(const ShaderModuleCreateInfo& aInfo) override;
		VkShaderModule getModule() const;

	private:
		VkShaderModule mModule;
};

#endif // vulkanshadermodule_h__