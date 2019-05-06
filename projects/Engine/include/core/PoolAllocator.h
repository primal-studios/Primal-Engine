#ifndef poolallocator_h__
#define poolallocator_h__

#include <cstdint>
#include <list>

class PoolAllocator
{
	public:
		PoolAllocator();
		PoolAllocator(const size_t aAlignment, const size_t aNumBlocks, const size_t aSizePerBlock);
		~PoolAllocator();

		void* getBlock();
		void freeBlock(void* aBlock);

	private:
		void* mMem;

		size_t mAlignment;
		size_t mNumBlocksTotal;
		size_t mSizeofBlock;

		std::list<void*> mFreeBlocks;
};

#endif // poolallocator_h__