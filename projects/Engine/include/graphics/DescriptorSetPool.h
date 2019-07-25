#ifndef descriptorsetpool_h__
#define descriptorsetpool_h__

#include <unordered_map>
#include <vector>

#include "graphics/api/IDescriptorPool.h"
#include "graphics/api/IDescriptorSet.h"

struct DescriptorSet
{
	IDescriptorSet* set;
	uint32_t index;
};

class DescriptorSetPool
{
	public:
		DescriptorSetPool(uint32_t aChunkSize, DescriptorPoolCreateInfo aInfo);
		DescriptorSetPool(const DescriptorSetPool&) = delete;
		DescriptorSetPool(DescriptorSetPool&&) noexcept = delete;
		~DescriptorSetPool();

		DescriptorSetPool& operator=(const DescriptorSetPool&) = delete;
		DescriptorSetPool& operator=(DescriptorSetPool&&) noexcept = delete;

		DescriptorSet acquire();
		DescriptorSet acquire(uint32_t aIndex);
		void release(DescriptorSet aObject);

		IDescriptorPool* getPool(DescriptorSet pSet);

	private:
		std::vector<IDescriptorPool*> mPools;
		std::vector<IDescriptorSet*> mSets;
		std::unordered_map<uint32_t, IDescriptorSet*> mFreeSlots;
		DescriptorPoolCreateInfo mCreateInfo;
		uint32_t mCursor;
		uint32_t mChunkSize;
};

#endif // descriptorsetpool_h__
