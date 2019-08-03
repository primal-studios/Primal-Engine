#include "graphics/api/IGraphicsPipeline.h"

IGraphicsPipeline::IGraphicsPipeline(IGraphicsContext* aContext)
{
	mContext = aContext;
}

IRenderPass* IGraphicsPipeline::getRenderPass() const
{
	return mCreateInfo.renderPass;
}
