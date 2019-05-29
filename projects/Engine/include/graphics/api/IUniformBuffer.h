#ifndef iuniformbuffer_h__
#define iuniformbuffer_h__

#include <cstdint>
#include <vector>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/ICommandPool.h"

#include "graphics/BufferLayout.h"
#include "graphics/BufferFlags.h"
#include "graphics/SharingMode.h"

struct UniformBufferCreateInfo
{
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

		virtual void setLayout(const BufferLayout& aLayout) = 0;

	private:
		IGraphicsContext* mContext;
};

#endif // iuniformbuffer_h__