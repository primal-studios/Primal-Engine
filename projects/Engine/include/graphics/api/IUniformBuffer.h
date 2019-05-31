#ifndef iuniformbuffer_h__
#define iuniformbuffer_h__


#include "graphics/BufferLayout.h"
#include "graphics/BufferFlags.h"
#include "graphics/SharingMode.h"
#include "graphics/ShaderStageFlags.h"
#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/ICommandPool.h"

#include <cstdint>
#include <vector>

enum EDescriptorType : uint32_t
{
	DESCRIPTOR_TYPE_SAMPLER = 0,
	DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER = 1,
	DESCRIPTOR_TYPE_SAMPLED_IMAGE = 2,
	DESCRIPTOR_TYPE_STORAGE_IMAGE = 3,
	DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER = 4,
	DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER = 5,
	DESCRIPTOR_TYPE_UNIFORM_BUFFER = 6,
	DESCRIPTOR_TYPE_STORAGE_BUFFER = 7,
	DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC = 8,
	DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC = 9,
	DESCRIPTOR_TYPE_INPUT_ATTACHMENT = 10,
};

struct UniformBufferCreateInfo
{
	EDescriptorType descriptorType;
	size_t size;

	EShaderStageFlags shaderStageFlags;

	EBufferCreateFlags flags;
	EBufferUsageFlags usage;
	ESharingMode sharingMode;
	std::vector<uint32_t> queueFamilyIndices;

	ICommandPool* commandPool;
};

class IUniformBuffer
{
	public:
		explicit IUniformBuffer(IGraphicsContext* aContext);
		IUniformBuffer(const IUniformBuffer& aOther) = delete;
		IUniformBuffer(IUniformBuffer&& aOther) noexcept = delete;
		virtual ~IUniformBuffer() = default;

		IUniformBuffer& operator = (const IUniformBuffer& aOther) = delete;
		IUniformBuffer& operator = (IUniformBuffer&& aOther) noexcept = delete;

		virtual void construct(const UniformBufferCreateInfo& aInfo) = 0;
		virtual void setData(void* aData, const size_t aSize) = 0;

		virtual void setLayout(const BufferLayout& aLayout) = 0;

	private:
		IGraphicsContext* mContext;
};

#endif // iuniformbuffer_h__