#ifndef vulkancommandbuffer_h__
#define vulkancommandbuffer_h__

#include "graphics/api/ICommandBuffer.h"

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

	void record(const CommandBufferRecordInfo& aInfo) override;
	void end() override;

	void recordRenderPass(const RenderPassRecordInfo& aInfo) override;
	void endRenderPass() override;

	void bindGraphicsPipeline(IGraphicsPipeline* aPipeline) override;

	void bindVertexBuffers(uint32_t aFirstBinding, uint32_t aBindingCount, std::vector<IVertexBuffer*> aBuffers, std::vector<uint64_t> aOffsets) override;
	void bindIndexBuffer(IIndexBuffer* aBuffer, uint64_t aOffset, EIndexType aType) override;

	void draw(uint32_t aVertexCount, uint32_t aInstanceCount, uint32_t aFirstVertex, uint32_t aFirstInstance) override;
	void drawIndexed(uint32_t aIndexCount, uint32_t aInstanceCount, uint32_t aFirstIndex, int32_t aVertexOffset, uint32_t aFirstInstance) override;

	VkCommandBuffer getHandle() const;
	VkSemaphore getSemaphore() const;

	std::vector<VkSemaphore>& getSemaphoresToSignal();
	std::vector<VkSemaphore>& getSemaphoresToWaitOn();
	VkFence& getFence();
private:
	void _destroy();

	IGraphicsContext* mContext;
	VkCommandBuffer mBuffer{};
	VkCommandPool mPool{};

	VkSemaphore mSemaphore{};
	VkFence mFence{};

	std::vector<VulkanCommandBuffer*> mDependsOnThis;
	std::vector<VulkanCommandBuffer*> mThisDependsOn;

	std::vector<VkSemaphore> mSemDependsOnThis;
	std::vector<VkSemaphore> mSemThisDependsOn;
};

#endif // vulkancommandbuffer_h__
