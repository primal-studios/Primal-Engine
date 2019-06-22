#include "graphics/UniformBufferPool.h"

static constexpr uint32_t UboSize = 1 << 16;

UniformBufferPool::UniformBufferPool(const uint32_t aChunkSize, const uint32_t aStride, const UniformBufferCreateInfo aUboCreateInfo)
	: mCreateInfo(aUboCreateInfo), mCursor(0), mChunkSize(aChunkSize), mStride(aStride)
{
	PRIMAL_ASSERT(aChunkSize * aUboCreateInfo.size < UboSize, "Requested a UBO that is too large.");
}

UniformBufferPool::~UniformBufferPool()
{
	for (auto buf : mBuffers)
	{
		delete buf;
	}
	mBuffers.clear();
	mFreeSlots.clear();
}

UniformBufferObject UniformBufferPool::acquire()
{
	return acquire(mCursor++);
}

UniformBufferObject UniformBufferPool::acquire(const uint32_t aIndex)
{
	if (aIndex < mCursor)
	{
		mFreeSlots.erase(aIndex);
		const uint32_t index = aIndex / mChunkSize;
		const uint32_t offset = aIndex - (index * mChunkSize);
		return UniformBufferObject{
			mBuffers[index],
			offset * mStride,
			mStride,
			aIndex
		};
	}
	mCursor = aIndex + 1;
	IUniformBuffer* ubo = GraphicsFactory::instance().createUniformBuffer();
	ubo->construct(mCreateInfo);
	mBuffers.push_back(ubo);

	const uint32_t index = aIndex / mChunkSize;
	const uint32_t offset = aIndex - (index * mChunkSize);
	return UniformBufferObject{
		mBuffers[index],
		offset * mStride,
		mStride,
		aIndex
	};
}

void UniformBufferPool::release(const UniformBufferObject aObject)
{
	const auto searchIt = std::find(mBuffers.begin(), mBuffers.end(), aObject.buffer);
	PRIMAL_ASSERT(searchIt != mBuffers.end(), "Found invalid buffer upon release.  The pool does not own this buffer.");
	PRIMAL_ASSERT(aObject.poolIndex < mCursor, "Invalid pool index in uniform buffer object.");
	mFreeSlots.insert(aObject.poolIndex);
}
