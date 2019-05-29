#ifndef vulkanindexbuffer_h__
#define vulkanindexbuffer_h__

#include "graphics/api/IIndexBuffer.h"

class VulkanIndexBuffer : public IIndexBuffer
{
	public:
		explicit VulkanIndexBuffer(IGraphicsContext* aContext);
		VulkanIndexBuffer(const VulkanIndexBuffer& aOther) = delete;
		VulkanIndexBuffer(VulkanIndexBuffer&& aOther) noexcept = delete;
		~VulkanIndexBuffer();

		VulkanIndexBuffer& operator = (const VulkanIndexBuffer& aOther) = delete;
		VulkanIndexBuffer& operator = (VulkanIndexBuffer&& aOther) noexcept = delete;

		void construct(const IndexBufferCreateInfo& aInfo) override;

		void setData(void* aData, const size_t aSize) override;

		void bind() override;
		void unbind() override;

	private:
		VkBuffer mBuffer;
		VkBuffer mStagingBuffer;
		VmaAllocation mStagingAllocation{};
		VmaAllocation mAllocation{};
};

#endif // vulkanindexbuffer_h__