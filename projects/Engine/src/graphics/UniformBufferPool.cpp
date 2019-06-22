#include <utility>
#include "graphics/UniformBufferPool.h"

static constexpr uint32_t UboSize = 1 << 16;

UniformBufferObject::UniformBufferObject(IUniformBuffer* aBuffer, const uint32_t aOffset, const uint32_t aSize, const uint32_t aPoolIndex, const std::vector<UniformBufferObjectElement*>& aElements)
	: mElements(aElements), mBuffer(aBuffer), mOffset(aOffset), mSize(aSize), mPoolIndex(aPoolIndex)
{
}

const std::vector<UniformBufferObjectElement*>& UniformBufferObject::getElements() const
{
	return mElements;
}

const UniformBufferObjectElement* UniformBufferObject::getElement(const std::string& aName) const
{
	for (const auto & element : mElements)
	{
		if (element->name == aName)
		{
			return element;
		}
	}
	return nullptr;
}

IUniformBuffer* UniformBufferObject::getBuffer() const
{
	return mBuffer;
}

uint32_t UniformBufferObject::getOffset() const
{
	return mOffset;
}

uint32_t UniformBufferObject::getSize() const
{
	return mSize;
}

uint32_t UniformBufferObject::getPoolIndex() const
{
	return mPoolIndex;
}

UniformBufferPool::UniformBufferPool(const uint32_t aChunkSize, const uint32_t aStride, const UniformBufferCreateInfo aUboCreateInfo,
                                     std::vector<UniformBufferObjectElement*> aBufferElements)
	: mCreateInfo(aUboCreateInfo), mBufferElements(std::move(aBufferElements)), mCursor(0), mChunkSize(aChunkSize), mStride(aStride)
{
	PRIMAL_ASSERT(aChunkSize * aUboCreateInfo.size < UboSize, "Requested a UBO that is too large.");
}

UniformBufferPool::~UniformBufferPool()
{
	for (auto bufferElement : mBufferElements)
	{
		delete bufferElement;
	}

	for (auto buf : mBuffers)
	{
		delete buf;
	}
	mBuffers.clear();
	mFreeSlots.clear();
}

UniformBufferObject* UniformBufferPool::acquire()
{
	return acquire(mCursor++);
}

UniformBufferObject* UniformBufferPool::acquire(const uint32_t aIndex)
{
	if (aIndex < mCursor)
	{
		mFreeSlots.erase(aIndex);
		const uint32_t index = aIndex / mChunkSize;
		const uint32_t offset = aIndex - (index * mChunkSize);
		return new UniformBufferObject(mBuffers[index], offset * mStride, mStride, aIndex, mBufferElements);
	}
	mCursor = aIndex + 1;
	IUniformBuffer* ubo = GraphicsFactory::instance().createUniformBuffer();
	ubo->construct(mCreateInfo);
	mBuffers.push_back(ubo);

	const uint32_t index = aIndex / mChunkSize;
	const uint32_t offset = aIndex - (index * mChunkSize);
	return new UniformBufferObject(mBuffers[index], offset * mStride, mStride, aIndex, mBufferElements);
}

void UniformBufferPool::release(UniformBufferObject* aObject)
{
	const auto searchIt = std::find(mBuffers.begin(), mBuffers.end(), aObject->getBuffer());
	PRIMAL_ASSERT(searchIt != mBuffers.end(), "Found invalid buffer upon release.  The pool does not own this buffer.");
	PRIMAL_ASSERT(aObject->getPoolIndex() < mCursor, "Invalid pool index in uniform buffer object.");
	mFreeSlots.insert(aObject->getPoolIndex());
}
