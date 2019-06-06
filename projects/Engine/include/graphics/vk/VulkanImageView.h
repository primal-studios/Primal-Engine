#ifndef vulkanimageview_h__
#define vulkanimageview_h__

#include "graphics/api/IImageView.h"

#include <vulkan/vulkan.h>

class VulkanImageView final : public IImageView
{
	public:
		explicit VulkanImageView(IGraphicsContext* aContext);
		VulkanImageView(const VulkanImageView&) = delete;
		VulkanImageView(VulkanImageView&&) noexcept = delete;
		~VulkanImageView() override;

		VulkanImageView& operator=(const VulkanImageView&) = delete;
		VulkanImageView& operator=(VulkanImageView&&) noexcept = delete;
		void construct(const ImageViewCreateInfo&) override;
		VkImageView getHandle() const;

	private:
		VkImageView mImageView{};
		IGraphicsContext* mContext;
};

#endif // vulkanimageview_h__