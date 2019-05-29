#include "graphics/api/IIndexBuffer.h"

IIndexBuffer::IIndexBuffer(IGraphicsContext* aContext)
{
	mSize = 0;
	mData = nullptr;
	mContext = aContext;
}