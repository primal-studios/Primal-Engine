#ifndef vulkanuniformbuffer_h__
#define vulkanuniformbuffer_h__

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "graphics/api/IUniformBuffer.h"

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
		void reconstruct(const UniformBufferCreateInfo& aInfo) override;
		void setData(void* aData, const size_t aSize, const size_t aCurrentImage) override;

	private:
		void _destroy();

		std::vector<VkBuffer> mBuffer;
		std::vector<VmaAllocation> mAllocation;

		VkDeviceSize mSize;
		UniformBufferCreateInfo mInfo;
};

#endif // vulkanuniformbuffer_h__