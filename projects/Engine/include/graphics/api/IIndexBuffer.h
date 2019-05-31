#ifndef iindexbuffer_h__
#define iindexbuffer_h__

#include "graphics/BufferFlags.h"
#include "graphics/SharingMode.h"
#include "graphics/api/ICommandPool.h"
#include "graphics/api/IGraphicsContext.h"

#include <cstdint>
#include <vector>

struct IndexBufferCreateInfo
{
	EBufferCreateFlags flags;
	EBufferUsageFlags usage;
	ESharingMode sharingMode;
	std::vector<uint32_t> queueFamilyIndices;

	ICommandPool* commandPool;
};

class IIndexBuffer
{
	public:
		explicit IIndexBuffer(IGraphicsContext* aContext);
		IIndexBuffer(const IIndexBuffer& aOther) = delete;
		IIndexBuffer(IIndexBuffer&& aOther) noexcept = delete;
		virtual ~IIndexBuffer() = default;

		IIndexBuffer& operator = (const IIndexBuffer& aOther) = delete;
		IIndexBuffer& operator = (IIndexBuffer&& aOther) noexcept = delete;

		virtual void construct(const IndexBufferCreateInfo& aInfo) = 0;

		virtual void setData(void* aData, const size_t aSize) = 0;

		virtual void bind() = 0;
		virtual void unbind() = 0;

	protected:
		IGraphicsContext* mContext;

		size_t mSize;
		void* mData;
};

#endif // iindexbuffer_h__