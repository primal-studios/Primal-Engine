#ifndef idescriptorset_h__
#define idescriptorset_h__

#include <vector>

#include "graphics/api/IGraphicsContext.h"

#include "graphics/api/IDescriptorPool.h"
#include "IDescriptorSetLayout.h"

class OffsetSize
{
	public:
		OffsetSize(const uint32_t aOffset, const uint32_t aSize) 
			: mOffset(aOffset), mSize(aSize)
		{
			
		}

		const uint32_t& getOffset() const { return mOffset; }
		const uint32_t& getSize() const { return mSize; }

		bool operator == (const OffsetSize& aOther) const
		{
			return mOffset == aOther.mOffset && mSize == aOther.mSize;
		}

		bool operator != (const OffsetSize& aOther) const
		{
			return !(*this == aOther);
		}

	private:
		uint32_t mOffset;
		uint32_t mSize;
};

struct DescriptorSetCreateInfo
{
	IDescriptorPool* pool;
	std::vector<IDescriptorSetLayout*> setLayouts;
};

class IDescriptorSet
{
	public:
		explicit IDescriptorSet(IGraphicsContext* aContext);
		IDescriptorSet(const IDescriptorSet&) = delete;
		IDescriptorSet(IDescriptorSet&&) noexcept = delete;
		virtual ~IDescriptorSet() = default;

		IDescriptorSet& operator=(const IDescriptorSet&) = delete;
		IDescriptorSet& operator=(IDescriptorSet&&) noexcept = delete;

		virtual void construct(const DescriptorSetCreateInfo& aInfo) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // idescriptorset_h__