#ifndef vulkancommandbuffer_h__
#define vulkancommandbuffer_h__

#include <graphics/api/ICommandBuffer.h>

#include <vector>
#include <vulkan/vulkan.h>

class VulkanCommandBuffer final : public ICommandBuffer
{
public:
	explicit VulkanCommandBuffer(IGraphicsContext* aContext);
	VulkanCommandBuffer(const VulkanCommandBuffer& aCommandBuffer) = delete;
	VulkanCommandBuffer(VulkanCommandBuffer&& aCommandBuffer) noexcept = delete;
	~VulkanCommandBuffer() override;

	VulkanCommandBuffer& operator=(const VulkanCommandBuffer&) = delete;
	VulkanCommandBuffer& operator=(VulkanCommandBuffer&&) noexcept = delete;

	void addDependency(ICommandBuffer* aDependsOn) override;
	void removeDependency(ICommandBuffer* aDependsOn) override;
	void construct(const CommandBufferCreateInfo& aInfo) override;
	void reconstruct(const CommandBufferCreateInfo& aInfo) override;

	VkCommandBuffer getHandle() const;
	VkSemaphore getSemaphore() const;
private:
	void _destroy() const;

	IGraphicsContext* mContext;
	VkCommandBuffer mBuffer{};
	VkCommandPool mPool{};
	VkSemaphore mSemaphore{};

	std::vector<VulkanCommandBuffer*> mDependsOnThis;
	std::vector<VulkanCommandBuffer*> mThisDependsOn;
};

#endif // vulkancommandbuffer_h__
