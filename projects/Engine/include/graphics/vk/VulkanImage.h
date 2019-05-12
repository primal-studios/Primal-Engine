#ifndef vulkanimage_h__
#define vulkanimage_h__

#include <graphics/api/IImage.h>

#include <vma/vk_mem_alloc.h>
#include <vulkan/vulkan.h>

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
	VkImage getHandle() const;
private:
	VkImage mImage{};
	IGraphicsContext* mContext;
	VmaAllocator mAllocator{};
	VmaAllocation mAllocation{};
};

#endif // vulkanimage_h__
