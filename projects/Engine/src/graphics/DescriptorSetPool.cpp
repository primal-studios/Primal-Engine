#include <utility>
#include "graphics/DescriptorSetPool.h"

#include "graphics/GraphicsFactory.h"

DescriptorSetPool::DescriptorSetPool(DescriptorPoolCreateInfo aInfo)
	: mCreateInfo(std::move(aInfo)), mCursor(0)
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

	const auto res = acquire(mCursor);
	++mCursor;
	return res;
}

DescriptorSet DescriptorSetPool::acquire(const uint32_t aIndex)
{
	if (aIndex < mCursor)
	{
		mFreeSlots.erase(aIndex);
		return DescriptorSet{ mSets[aIndex], aIndex };
	}
	auto descPool = GraphicsFactory::instance().createDescriptorPool();
	descPool->construct(mCreateInfo);
	mPools.push_back(descPool);

	IDescriptorSet* desc = GraphicsFactory::instance().createDescriptorSet();

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

IDescriptorPool* DescriptorSetPool::getPool(const DescriptorSet pSet)
{
	if (pSet.index < mCursor)
	{
		return mPools[pSet.index];
	}
	return nullptr;
}
