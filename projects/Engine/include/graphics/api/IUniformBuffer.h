#ifndef iuniformbuffer_h__
#define iuniformbuffer_h__


#include "graphics/BufferLayout.h"
#include "graphics/BufferFlags.h"
#include "graphics/SharingMode.h"
#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IDescriptorSetLayout.h"
#include "graphics/api/IDescriptorPool.h"

#include <cstdint>
#include <vector>

struct UniformBufferCreateInfo
{
	size_t size;

	BufferCreateFlags flags;
	BufferUsageFlags usage;
	ESharingMode sharingMode;
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

		virtual void setData(void* aData, const size_t aOffset) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // iuniformbuffer_h__