#ifndef vulkanvertexbuffer_h__
#define vulkanvertexbuffer_h__

#include "graphics/api/IVertexBuffer.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

class VulkanVertexBuffer final : public IVertexBuffer
{
	public:
		explicit VulkanVertexBuffer(IGraphicsContext* aContext);
		VulkanVertexBuffer(const VulkanVertexBuffer& aOther) = delete;
		VulkanVertexBuffer(VulkanVertexBuffer&& aOther) noexcept = delete;
		virtual ~VulkanVertexBuffer() override;

		VulkanVertexBuffer& operator = (const VulkanVertexBuffer& aOther) = delete;
		VulkanVertexBuffer& operator = (VulkanVertexBuffer&& aOther) noexcept = delete;

		void construct(const VertexBufferCreateInfo& aInfo) override;

		void setData(void* aData, const size_t aSize) override;
		void setLayout(const BufferLayout& aLayout) override;

		void bind() override;
		void unbind() override;

	private:
		VkBuffer mBuffer;
		VkBuffer mStagingBuffer;
		VmaAllocation mStagingAllocation{};
		VmaAllocation mAllocation{};

		VkVertexInputBindingDescription mBindingDescription;
		std::vector<VkVertexInputAttributeDescription> mAttributeDescriptions;

		
};

#endif // vulkanvertexbuffer_h__