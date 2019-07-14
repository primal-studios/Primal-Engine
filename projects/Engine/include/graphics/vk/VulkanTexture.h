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
		friend class VulkanCommandBuffer;
	public:
		explicit VulkanTexture(IGraphicsContext* aContext);
		VulkanTexture(const VulkanTexture&) = delete;
		VulkanTexture(VulkanTexture&&) noexcept = delete;
		~VulkanTexture() override;

		VulkanTexture& operator=(const VulkanTexture&) = delete;
		VulkanTexture& operator=(VulkanTexture&&) noexcept = delete;

		void construct(const TextureCreateInfo& aInfo) override;

		static DescriptorSetLayoutBinding getDescriptorSetLayout(const uint32_t aBinding, const VkShaderStageFlags aStage, const uint32_t aCount);

		WriteDescriptorSet getWriteDescriptor(const uint32_t aBinding, const std::optional<OffsetSize>& aOffsetSize) const;
		ShaderStageFlags getStageFlags() const override;
		uint32_t getBindingPoint() const override;
	private:
		VulkanImage* mImage;
		VulkanImageView* mImageView;
		VulkanSampler* mSampler;

		ShaderStageFlags mStages;
		uint32_t mBindingPoint;
};

#endif // vulkantexture_h__