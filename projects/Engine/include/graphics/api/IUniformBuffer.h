#ifndef iuniformbuffer_h__
#define iuniformbuffer_h__


#include "graphics/BufferLayout.h"
#include "graphics/BufferFlags.h"
#include "graphics/SharingMode.h"
#include "graphics/ShaderStageFlags.h"
#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/ICommandPool.h"
#include "graphics/api/ISampler.h"
#include "graphics/api/IDescriptorSets.h"

#include <cstdint>
#include <vector>

struct UniformBufferCreateInfo
{
	size_t size;

	uint32_t binding;

	uint32_t descriptorCount;
	uint32_t framesInFlight;

	std::vector<ISampler*> immutableSamplers;

	ShaderStageFlags shaderStageFlags;

	BufferCreateFlags flags;
	BufferUsageFlags usage;
	ESharingMode sharingMode;
	std::vector<uint32_t> queueFamilyIndices;

	IDescriptorSets* descriptorSets;

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
		virtual void reconstruct(const UniformBufferCreateInfo& aInfo) = 0;
		virtual void setData(void* aData, const size_t aSize, const size_t aCurrentImage) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // iuniformbuffer_h__