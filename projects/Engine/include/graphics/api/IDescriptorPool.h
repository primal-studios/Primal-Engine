#ifndef idescriptorpool_h__
#define idescriptorpool_h__

#include <vector>

#include "graphics/api/IGraphicsContext.h"
#include "graphics/api/IDescriptorSetLayout.h"

enum EDescriptorPoolCreateFlagBits : uint32_t
{
	
};

using DescriptorPoolCreateFlags = uint32_t;

struct DescriptorPoolSize
{
	EDescriptorType type;
	uint32_t count;
};

struct DescriptorPoolCreateInfo
{
	DescriptorPoolCreateFlags flags;

	uint32_t maxSets;

	std::vector<DescriptorPoolSize> poolSizes;
};

class IDescriptorPool
{
	public:
		explicit IDescriptorPool(IGraphicsContext* aContext);
		IDescriptorPool(const IDescriptorPool&) = delete;
		IDescriptorPool(IDescriptorPool&&) noexcept = delete;
		virtual ~IDescriptorPool() = default;

		IDescriptorPool& operator=(const IDescriptorPool&) = delete;
		IDescriptorPool& operator=(IDescriptorPool&&) noexcept = delete;

		virtual void construct(const DescriptorPoolCreateInfo& aInfo) = 0;

	protected:
		IGraphicsContext* mContext;
};

#endif // idescriptorpool_h__