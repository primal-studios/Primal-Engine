#ifndef vulkantexture_h__
#define vulkantexture_h__

#include "graphics/api/ITexture.h"
#include "graphics/vk/VulkanImageView.h"
#include "graphics/vk/VulkanImage.h"
#include "graphics/api/IDescriptorSetLayout.h"
#include "graphics/api/IDescriptorSet.h"
#include "graphics/vk/VulkanSampler.h"

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

		IDescriptorSetLayout* getLayout() const;
		IDescriptorSet* getSet() const;

	private:
		VulkanImage* mImage;
		VulkanImageView* mImageView;
		VulkanSampler* mSampler;

		IDescriptorSetLayout* mLayout;
		IDescriptorSet* mSets;
};

#endif // vulkantexture_h__