#ifndef vulkanimage_h__
#define vulkanimage_h__

#include "graphics/api/IImage.h"

#include <vulkan/vulkan.h>
#include <vma/vk_mem_alloc.h>

class VulkanImage final : public IImage
{
public:
	explicit VulkanImage(IGraphicsContext* aContext);
	VulkanImage(const VulkanImage&) = delete;
	VulkanImage(VulkanImage&&) noexcept = delete;
	~VulkanImage() override;

	VulkanImage& operator=(const VulkanImage&) = delete;
	VulkanImage& operator=(VulkanImage&&) noexcept = delete;

	void construct(const ImageCreateInfo&) override;
	void reconstruct(const ImageCreateInfo&) override;
	VkImage getHandle() const;

	void setHandle(VkImage aImage);
	void setData(void* aData, const size_t aSize) override;
private:
	void _transitionToLayout(const ImageCreateInfo& aInfo, VkFormat aFormat, VkImageLayout aOldLayout, VkImageLayout aNewLayout) const;
	void _copyBufferToImage(const ImageCreateInfo& aInfo) const;

	void _destroy() const;

	bool mOwning = true;
	VkImage mImage{};
	IGraphicsContext* mContext;
	VmaAllocator mAllocator{};
	VmaAllocation mAllocation{};

	VkBuffer mStagingBuffer = VK_NULL_HANDLE;
	VmaAllocation mStagingAllocation{};
	void* mStagingMemory = nullptr;
	size_t mStagingSize{};
};

#endif // vulkanimage_h__
