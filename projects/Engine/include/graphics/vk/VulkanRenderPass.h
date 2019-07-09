#ifndef vulkanrenderpass_h__
#define vulkanrenderpass_h__

#include "graphics/api/IRenderPass.h"

#include <vulkan/vulkan.h>

class VulkanRenderPass final : public IRenderPass
{
	public:
		explicit VulkanRenderPass(IGraphicsContext* aContext);
		VulkanRenderPass(const VulkanRenderPass&) = delete;
		VulkanRenderPass(VulkanRenderPass&&) noexcept = delete;
		~VulkanRenderPass() override;

		VulkanRenderPass& operator=(const VulkanRenderPass&) = delete;
		VulkanRenderPass& operator=(VulkanRenderPass&&) noexcept = delete;

		void construct(const RenderPassCreateInfo& aInfo) override;
		void reconstruct(const RenderPassCreateInfo& aInfo) override;
		void destroy() override;
		
		RenderPassCreateInfo& getCreateInfo() override;
		VkRenderPass getHandle() const;
	private:
		void _destroy() const;

		RenderPassCreateInfo mCreateInfo;

		IGraphicsContext* mContext;
		VkRenderPass mPass{};

};

#endif // vulkanrenderpass_h__
