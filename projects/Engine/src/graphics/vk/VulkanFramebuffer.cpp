#include "core/Log.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanFramebuffer.h"
#include "graphics/vk/VulkanImageView.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanRenderPass.h"

VulkanFramebuffer::VulkanFramebuffer(IGraphicsContext* aContext)
	: IFramebuffer(aContext), mContext(aContext)
{
}

VulkanFramebuffer::~VulkanFramebuffer()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyFramebuffer(context->getDevice(), mBuffer, nullptr);
}

void VulkanFramebuffer::construct(const FramebufferCreateInfo& aInfo)
{
	using std::vector;
	
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	VulkanRenderPass* rp = primal_cast<VulkanRenderPass*>(aInfo.renderPass);

	vector<VkImageView> attachments;
	attachments.reserve(aInfo.attachments.size());

	for (auto & a : aInfo.attachments)
	{
		VulkanImageView* view = reinterpret_cast<VulkanImageView*>(a);
		attachments.emplace_back(view->getHandle());
	}
	
	VkFramebufferCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	info.flags = 0;
	info.width = aInfo.width;
	info.height = aInfo.height;
	info.layers = aInfo.layers;
	info.renderPass = rp->getHandle();
	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.pAttachments = attachments.data();

	const VkResult res = vkCreateFramebuffer(context->getDevice(), &info, nullptr, &mBuffer);

	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan frame buffer.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan frame buffer.");
	}
}

void VulkanFramebuffer::reconstruct(const FramebufferCreateInfo& aInfo)
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyFramebuffer(context->getDevice(), mBuffer, nullptr);

	construct(aInfo);
}

VkFramebuffer VulkanFramebuffer::getHandle() const
{
	return mBuffer;
}
