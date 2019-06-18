#ifndef vulkantexture_h__
#define vulkantexture_h__

#include <optional>

#include "graphics/api/ITexture.h"
#include "graphics/vk/VulkanImageView.h"
#include "graphics/vk/VulkanImage.h"
#include "graphics/api/IDescriptorSetLayout.h"
#include "graphics/api/IDescriptorSet.h"
#include "graphics/vk/VulkanSampler.h"
#include "VulkanDescriptorSet.h"

class VulkanTexture final : public ITexture
{
	public:
		explicit VulkanTexture(IGraphicsContext* aContext);
		VulkanTexture(const VulkanTexture&) = delete;
		VulkanTexture(VulkanTexture&&) noexcept = delete;
		~VulkanTexture() override;

		VulkanTexture& operator=(const VulkanTexture&) = delete;
		VulkanTexture& operator=(VulkanTexture&&) noexcept = delete;

		void construct(const TextureCreateInfo& aInfo) override;

		void bind(const uint32_t aBinding) override;

		static DescriptorSetLayoutBinding getDescriptorSetLayout(const uint32_t aBinding, const VkShaderStageFlags aStage, const uint32_t aCount);

		WriteDescriptorSet getWriteDescriptor(const uint32_t aBinding, const std::optional<OffsetSize>& aOffsetSize) const;

	private:
		VulkanImage* mImage;
		VulkanImageView* mImageView;
		VulkanSampler* mSampler;
};

#endif // vulkantexture_h__