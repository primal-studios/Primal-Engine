#include "graphics/DescriptorSetPool.h"

#include "graphics/GraphicsFactory.h"

DescriptorSetPool::DescriptorSetPool(const uint32_t aChunkSize, const DescriptorPoolCreateInfo& aInfo, const std::vector<IDescriptorSetLayout*> aLayouts)
	: mLayouts(aLayouts), mCreateInfo(aInfo), mCursor(0), mChunkSize(aChunkSize)
{
}

DescriptorSetPool::~DescriptorSetPool()
{
	for (auto set : mSets)
	{
		delete set;
	}

	for (auto buffer : mPools)
	{
		delete buffer;
	}

	mSets.clear();
	mPools.clear();
}

DescriptorSet DescriptorSetPool::acquire()
{
	if (!mFreeSlots.empty())
	{
		const uint32_t slot = mFreeSlots.begin()->first;
		IDescriptorSet* set = mFreeSlots.begin()->second;
		mFreeSlots.erase(slot);
		return DescriptorSet{ set, slot };
	}

	return acquire(mCursor++);
}

DescriptorSet DescriptorSetPool::acquire(const uint32_t aIndex)
{
	const uint32_t index = aIndex / mChunkSize;
	const uint32_t offset = aIndex - (index * mChunkSize);
	const DescriptorSetCreateInfo info = { mPools[index], mLayouts };

	if (aIndex < mCursor)
	{
		mFreeSlots.erase(aIndex);
		return DescriptorSet{ mSets[aIndex], aIndex };
	}

	IDescriptorSet* desc = GraphicsFactory::instance().createDescriptorSet();
	desc->construct(info);

	if (mSets.size() <= aIndex)
	{
		mSets.resize(static_cast<size_t>(aIndex) + 1);
	}

	mSets[aIndex] = desc;

	const DescriptorSet set = { desc, aIndex };
	return set;
}

void DescriptorSetPool::release(DescriptorSet aObject)
{
	mFreeSlots.insert({ aObject.index, aObject.set });
}
