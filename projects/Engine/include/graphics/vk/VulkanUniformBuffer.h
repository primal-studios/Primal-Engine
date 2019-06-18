#ifndef vulkanuniformbuffer_h__
#define vulkanuniformbuffer_h__

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

#include "graphics/api/IUniformBuffer.h"
#include "graphics/api/IDescriptorSet.h"
#include <optional>
#include "VulkanDescriptorSet.h"

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

		void setData(void* aData, const size_t aOffset) override;

		static DescriptorSetLayoutBinding getDescriptorSetLayout(const uint32_t aBinding, const VkShaderStageFlags aStage, const uint32_t aCount);

		WriteDescriptorSet getWriteDescriptor(const uint32_t aBinding, const std::optional<OffsetSize>& aOffsetSize) const;

	private:
		void _destroy() const;

		VkBuffer mBuffer;
		VmaAllocation mAllocation;

		VkDeviceSize mSize;
		UniformBufferCreateInfo mInfo;
};

#endif // vulkanuniformbuffer_h__