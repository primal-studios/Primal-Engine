#include "core/PoolAllocator.h"

constexpr size_t DEFAULT_ALIGNMENT = 64;

#if !defined(PRIMAL_PLATFORM_WINDOWS)
#include <stdlib.h>
#else
#include <malloc.h>
#endif


PoolAllocator::PoolAllocator()
{
	mAlignment = DEFAULT_ALIGNMENT;
	mNumBlocksTotal = 0;
	mSizeofBlock = 0;
	mMem = nullptr;
}

PoolAllocator::PoolAllocator(const size_t aAlignment, const size_t aNumBlocks, const size_t aSizePerBlock)
{
	mAlignment = aAlignment;
	mNumBlocksTotal = aNumBlocks;
	mSizeofBlock = aSizePerBlock;


#if !defined(PRIMAL_PLATFORM_WINDOWS)
	mMem = aligned_alloc(mAlignment, mNumBlocksTotal * mSizeofBlock);
#else
	mMem = _aligned_malloc(mNumBlocksTotal * mSizeofBlock, mAlignment);
#endif

	void* mover = mMem;
	for (uint32_t i = 0; i < mNumBlocksTotal; ++i)
	{
		mFreeBlocks.push_back(mover);
		mover = static_cast<uint8_t*>(mover) + mSizeofBlock;
	}

}

PoolAllocator::~PoolAllocator()
{
	if (mMem)
	{
#if !defined(PRIMAL_PLATFORM_WINDOWS)
		free(mMem);
#else
		_aligned_free(mMem);
#endif
		mMem = nullptr;
	}

	mFreeBlocks.clear();

}

void* PoolAllocator::getBlock()
{
	void* freeMem = mFreeBlocks.back();
	mFreeBlocks.pop_back();

	return freeMem;

}

void PoolAllocator::freeBlock(void* aBlock)
{
	mFreeBlocks.push_front(aBlock);
}
