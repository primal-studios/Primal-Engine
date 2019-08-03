#ifndef vulkanindexbuffer_h__
#define vulkanindexbuffer_h__

#include "graphics/api/IIndexBuffer.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

class VulkanIndexBuffer final : public IIndexBuffer
{
		friend class VulkanCommandBuffer;
	public:
		explicit VulkanIndexBuffer(IGraphicsContext* aContext);
		VulkanIndexBuffer(const VulkanIndexBuffer& aOther) = delete;
		VulkanIndexBuffer(VulkanIndexBuffer&& aOther) noexcept = delete;
		virtual ~VulkanIndexBuffer() override;

		VulkanIndexBuffer& operator = (const VulkanIndexBuffer& aOther) = delete;
		VulkanIndexBuffer& operator = (VulkanIndexBuffer&& aOther) noexcept = delete;

		void construct(const IndexBufferCreateInfo& aInfo) override;

		[[nodiscard]] uint32_t getCount() const override;
	private:
		VkBuffer mBuffer = VK_NULL_HANDLE;
		VkBuffer mStagingBuffer = VK_NULL_HANDLE;
		VmaAllocation mStagingAllocation{};
		VmaAllocation mAllocation{};

		IndexBufferCreateInfo mInfo;
};

#endif // vulkanindexbuffer_h__