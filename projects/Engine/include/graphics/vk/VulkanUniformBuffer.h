#ifndef vulkanuniformbuffer_h__
#define vulkanuniformbuffer_h__

#include "graphics/api/IUniformBuffer.h"

class VulkanUniformBuffer : public IUniformBuffer
{
	public:
		explicit VulkanUniformBuffer(IGraphicsContext* aContext);
		VulkanUniformBuffer(const VulkanUniformBuffer& aOther) = delete;
		VulkanUniformBuffer(VulkanUniformBuffer&& aOther) noexcept = delete;
		virtual ~VulkanUniformBuffer();

		VulkanUniformBuffer& operator = (const VulkanUniformBuffer& aOther) = delete;
		VulkanUniformBuffer& operator = (VulkanUniformBuffer&& aOther) noexcept = delete;

		void construct(const UniformBufferCreateInfo& aInfo) override;

		void setLayout(const BufferLayout& aLayout) override;
};

#endif // vulkanuniformbuffer_h__