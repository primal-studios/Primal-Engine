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

class IDescriptorSets
{
	public:
		explicit IDescriptorSets(IGraphicsContext* aContext);
		IDescriptorSets(const IDescriptorSets&) = delete;
		IDescriptorSets(IDescriptorSets&&) noexcept = delete;
		virtual ~IDescriptorSets() = default;

		IDescriptorSets& operator=(const IDescriptorSets&) = delete;
		IDescriptorSets& operator=(IDescriptorSets&&) noexcept = delete;

		virtual void construct(const DescriptorSetCreateInfo& aInfo) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // idescriptorset_h__