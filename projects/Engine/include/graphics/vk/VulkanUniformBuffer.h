#ifndef vulkanuniformbuffer_h__
#define vulkanuniformbuffer_h__

#include "graphics/api/IUniformBuffer.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

class VulkanUniformBuffer final : public IUniformBuffer
{
	public:
		explicit VulkanUniformBuffer(IGraphicsContext* aContext);
		VulkanUniformBuffer(const VulkanUniformBuffer& aOther) = delete;
		VulkanUniformBuffer(VulkanUniformBuffer&& aOther) noexcept = delete;
		virtual ~VulkanUniformBuffer() override;

		VulkanUniformBuffer& operator = (const VulkanUniformBuffer& aOther) = delete;
		VulkanUniformBuffer& operator = (VulkanUniformBuffer&& aOther) noexcept = delete;

		void construct(const UniformBufferCreateInfo& aInfo) override;
		void setData(void* aData, const size_t aSize) override;

		void setLayout(const BufferLayout& aLayout) override;
};

#endif // vulkanuniformbuffer_h__