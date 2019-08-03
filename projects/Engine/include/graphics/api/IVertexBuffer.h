#ifndef ivertexbuffer_h__
#define ivertexbuffer_h__

#include "graphics/BufferLayout.h"
#include "graphics/BufferFlags.h"
#include "graphics/SharingMode.h"
#include "graphics/api/IGraphicsContext.h"

#include <cstdint>
#include <vector>

struct VertexBufferCreateInfo
{
	BufferCreateFlags flags;
	BufferUsageFlags usage;
	ESharingMode sharingMode;
	std::vector<uint32_t> queueFamilyIndices;
	size_t size;
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

		virtual void setLayout(const BufferLayout& aLayout) = 0;

	protected:
		IGraphicsContext* mContext;

		size_t mSize;
		void* mData;

		BufferLayout mLayout;
};

#endif // ivertexbuffer_h__