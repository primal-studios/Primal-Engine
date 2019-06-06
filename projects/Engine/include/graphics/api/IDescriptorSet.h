#ifndef idescriptorset_h__
#define idescriptorset_h__

#include <vector>

#include "graphics/api/IGraphicsContext.h"

#include "graphics/api/IDescriptorPool.h"
#include "IDescriptorSetLayout.h"

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