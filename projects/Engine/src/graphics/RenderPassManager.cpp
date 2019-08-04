#include "graphics/RenderPassManager.h"
#include "core/Log.h"

RenderPassManager& RenderPassManager::instance()
{
	static RenderPassManager* instance = new RenderPassManager();
	return *instance;
}

void RenderPassManager::registerRenderPass(const RenderPassLayout& aLayout, IRenderPass* aPass)
{
	if(mRenderPasses.find(aLayout) != mRenderPasses.end())
	{
		PRIMAL_WARN("RenderPassManager already contains a pass with this layout");
		return;
	}

	mRenderPasses[aLayout] = aPass;
}

IRenderPass* RenderPassManager::getRenderPass(const RenderPassLayout& aLayout)
{
	if (mRenderPasses.find(aLayout) == mRenderPasses.end())
	{
		PRIMAL_WARN("RenderPassManager does not contains a pass with this layout");
		return nullptr;
	}

	return mRenderPasses[aLayout];
}



