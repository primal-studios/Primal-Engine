#include "graphics/RenderPassManager.h"
#include "graphics/GraphicsFactory.h"

RenderPassManager& RenderPassManager::instance()
{
	static RenderPassManager manager = RenderPassManager();
	return manager;
}

RenderPassManager::~RenderPassManager()
{
	for (const auto & it : mRenderPassMap)
	{
		delete it.second;
	}
}

IRenderPass* RenderPassManager::create(const std::string& aName, const RenderPassCreateInfo& aCreateInfo)
{
	IRenderPass* rp = GraphicsFactory::instance().createRenderPass();
	rp->construct(aCreateInfo);
	mRenderPassMap.insert({ aName, rp });

	return rp;
}

IRenderPass* RenderPassManager::findPass(const std::string& aName) const
{
	const auto it = mRenderPassMap.find(aName);
	if (it == mRenderPassMap.end())
	{
		return nullptr;
	}
	return it->second;
}
