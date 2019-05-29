#ifndef vulkanframebuffer_h__
#define vulkanframebuffer_h__

#include <graphics/api/IFramebuffer.h>

#include <vulkan/vulkan.h>

class VulkanFramebuffer final : public IFramebuffer
{
	public:
		explicit VulkanFramebuffer(IGraphicsContext* aContext);
		VulkanFramebuffer(const VulkanFramebuffer&) = delete;
		VulkanFramebuffer(VulkanFramebuffer&&) noexcept = delete;
		~VulkanFramebuffer() override;

		VulkanFramebuffer& operator=(const VulkanFramebuffer&) = delete;
		VulkanFramebuffer& operator=(VulkanFramebuffer&&) noexcept = delete;

		void construct(const FramebufferCreateInfo&) override;
		void reconstruct(const FramebufferCreateInfo&) override;
		VkFramebuffer getHandle() const;
	private:
		IGraphicsContext* mContext;
		VkFramebuffer mBuffer{};
};

#endif // vulkanframebuffer_h__
