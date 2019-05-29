#ifndef ivertexbuffer_h__
#define ivertexbuffer_h__

#include <cstdint>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/VertexBufferLayout.h"
#include "graphics/api/ICommandPool.h"

#include "graphics/BufferFlags.h"
#include "graphics/SharingMode.h"

struct VertexBufferCreateInfo
{
	EBufferCreateFlags flags;
	EBufferUsageFlags usage;
	ESharingMode sharingMode;
	std::vector<uint32_t> queueFamilyIndices;

	ICommandPool* commandPool;
};

class IVertexBuffer
{
	public:
		explicit IVertexBuffer(IGraphicsContext* aContext);
		IVertexBuffer(const IVertexBuffer& aOther) = delete;
		IVertexBuffer(IVertexBuffer&& aOther) noexcept = delete;
		virtual ~IVertexBuffer() = default;

		IVertexBuffer& operator = (const IVertexBuffer& aOther) = delete;
		IVertexBuffer& operator = (IVertexBuffer&& aOther) noexcept = delete;

		virtual void construct(const VertexBufferCreateInfo& aInfo) = 0;

		virtual void setData(void* aData, const size_t aSize) = 0;
		virtual void setLayout(const VertexBufferLayout& aLayout) = 0;

		virtual void bind() = 0;
		virtual void unbind() = 0;

	protected:
		IGraphicsContext* mContext;

		size_t mSize;
		void* mData;

		VertexBufferLayout mLayout;
};

#endif // ivertexbuffer_h__