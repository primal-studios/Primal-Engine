#ifndef renderpassmanager_h__
#define renderpassmanager_h__

#include <unordered_map>

#include "api/IRenderPass.h"

struct RenderPassLayout
{
	explicit RenderPassLayout(const std::vector<size_t>& aAttachments)
	{
		attachments = aAttachments;
	}

	std::vector<size_t> attachments;

	inline bool operator == (const RenderPassLayout& aOther) const
	{
		if (attachments.size() != aOther.attachments.size())
			return false;

		for(size_t i = 0; i < attachments.size(); i++)
		{
			if (attachments[i] != aOther.attachments[i])
				return false;
		}

		return true;
	}

	inline bool operator != (const RenderPassLayout& aOther) const
	{
		if (attachments.size() != aOther.attachments.size())
			return true;

		for (size_t i = 0; i < attachments.size(); i++)
		{
			if (attachments[i] != aOther.attachments[i])
				return true;
		}

		return false;
	}
};

namespace std  // NOLINT(cert-dcl58-cpp)
{
	template<>
	struct hash<RenderPassLayout>
	{
		size_t operator() (const RenderPassLayout& aKey) const noexcept;
	};

	inline size_t hash<RenderPassLayout>::operator()(const RenderPassLayout& aKey) const noexcept
	{
		size_t h = 0;
		for (size_t attachment : aKey.attachments)
		{
			h += attachment;
		}

		return h;
	}
}

class RenderPassManager final
{
	public:
		static RenderPassManager& instance();

		void registerRenderPass(const RenderPassLayout& aLayout, IRenderPass* aPass);
		IRenderPass* getRenderPass(const RenderPassLayout& aLayout);

	private:
		RenderPassManager() = default;
		std::unordered_map<RenderPassLayout, IRenderPass*> mRenderPasses;
};

#endif // renderpassmanager_h__