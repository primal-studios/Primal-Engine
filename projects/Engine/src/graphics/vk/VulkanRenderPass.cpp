#include "core/Log.h"
#include "core/PrimalAssert.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanRenderPass.h"

#include <vector>

VulkanRenderPass::VulkanRenderPass(IGraphicsContext* aContext)
	: IRenderPass(aContext), mContext(aContext)
{
}

VulkanRenderPass::~VulkanRenderPass()
{
	_destroy();
}

void VulkanRenderPass::construct(const RenderPassCreateInfo& aInfo)
{
	using std::vector;
	
	vector<VkAttachmentDescription> attachments;
	vector<VkSubpassDependency> dependencies;
	vector<VkSubpassDescription> subpasses;

	vector<vector<VkAttachmentReference>> inputAttachments;
	vector<vector<VkAttachmentReference>> colorAttachments;
	vector<vector<VkAttachmentReference>> resolveAttachments;
	vector<VkAttachmentReference> depthStencils;
	vector<vector<uint32_t>> preserveAttachments;

	for (const auto & attachment : aInfo.attachments)
	{
		VkAttachmentDescription desc = {};
		desc.finalLayout = static_cast<VkImageLayout>(attachment.final);
		desc.format = static_cast<VkFormat>(attachment.format);
		desc.initialLayout = static_cast<VkImageLayout>(attachment.initial);
		desc.loadOp = static_cast<VkAttachmentLoadOp>(attachment.load);
		desc.storeOp = static_cast<VkAttachmentStoreOp>(attachment.store);
		desc.stencilLoadOp = static_cast<VkAttachmentLoadOp>(attachment.loadStencil);
		desc.stencilStoreOp = static_cast<VkAttachmentStoreOp>(attachment.storeStencil);
		desc.samples = static_cast<VkSampleCountFlagBits>(attachment.samples);

		attachments.push_back(desc);
	}

	for (const auto & subpass : aInfo.descriptions)
	{
		vector<VkAttachmentReference> inputs;
		vector<VkAttachmentReference> colors;
		vector<VkAttachmentReference> resolves;
		vector<uint32_t> preserve;

		for (const auto& color : subpass.colors)
		{
			VkAttachmentReference col = {};
			col.attachment = color.index;
			col.layout = static_cast<VkImageLayout>(color.layout);
			colors.push_back(col);
		}

		for (const auto& input : subpass.inputs)
		{
			VkAttachmentReference in = {};
			in.attachment = input.index;
			in.layout = static_cast<VkImageLayout>(input.layout);
			inputs.push_back(in);
		}

		for (const auto& resolve : subpass.resolve)
		{
			VkAttachmentReference res = {};
			res.attachment = resolve.index;
			res.layout = static_cast<VkImageLayout>(res.layout);
			resolves.push_back(res);
		}

		preserve = subpass.preserve;

		VkAttachmentReference depth = {
			subpass.depthStencil.index,
			static_cast<VkImageLayout>(subpass.depthStencil.layout)
		};

		inputAttachments.push_back(inputs);
		colorAttachments.push_back(colors);
		resolveAttachments.push_back(resolves);
		depthStencils.push_back(depth);
		preserveAttachments.push_back(preserve);
	}

	size_t i = 0;

	for (const auto& subpass : aInfo.descriptions)
	{
		VkSubpassDescription desc = {};
		desc.flags = 0;
		
		desc.colorAttachmentCount = static_cast<uint32_t>(colorAttachments[i].size());
		if (!colorAttachments[i].empty())
		{
			desc.pColorAttachments = colorAttachments[i].data();
		}
		else
		{
			desc.pColorAttachments = nullptr;
		}
		
		if (depthStencils[i].attachment == 0xFFFFFFFF)
		{
			desc.pDepthStencilAttachment = nullptr;
		}
		else
		{
			desc.pDepthStencilAttachment = &depthStencils[i];
		}
		
		desc.inputAttachmentCount = static_cast<uint32_t>(inputAttachments[i].size());
		if (!inputAttachments[i].empty())
		{
			desc.pInputAttachments = inputAttachments[i].data();
		}
		else
		{
			desc.pInputAttachments = nullptr;
		}

		PRIMAL_ASSERT(resolveAttachments[i].empty() || resolveAttachments[i].size() != colorAttachments[i].size(), 
			"Illegal number of resolve attachments.  Must be 0 or equal to the number of color attachments.");

		if (resolveAttachments[i].empty())
		{
			desc.pResolveAttachments = nullptr;
		}
		else
		{
			desc.pResolveAttachments = resolveAttachments[i].data();
		}

		desc.preserveAttachmentCount = static_cast<uint32_t>(preserveAttachments[i].size());
		if (preserveAttachments[i].empty())
		{
			desc.pPreserveAttachments = nullptr;
		}
		else
		{
			desc.pPreserveAttachments = preserveAttachments[i].data();
		}

		subpasses.push_back(desc);
	}

	for (const auto& dependency : aInfo.dependencies)
	{
		VkSubpassDependency dep = {};
		dep.dependencyFlags = dependency.dependencyFlags;
		dep.dstAccessMask = dependency.dstAccess;
		dep.dstStageMask = dependency.dstStages;
		dep.dstSubpass = dependency.dstSubPass;
		dep.srcAccessMask = dependency.srcAccess;
		dep.srcStageMask = dependency.srcStages;
		dep.srcSubpass = dependency.srcSubPass;

		dependencies.push_back(dep);
	}

	VkRenderPassCreateInfo info = {};
	info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	info.pNext = nullptr;
	info.flags = 0;

	info.attachmentCount = static_cast<uint32_t>(attachments.size());
	info.dependencyCount = static_cast<uint32_t>(dependencies.size());
	info.subpassCount = static_cast<uint32_t>(subpasses.size());

	info.pAttachments = attachments.data();
	info.pDependencies = dependencies.data();
	info.pSubpasses = subpasses.data();

	const auto context = primal_cast<VulkanGraphicsContext*>(mContext);
	const VkResult res = vkCreateRenderPass(context->getDevice(), &info, nullptr, &mPass);

	if (res != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to create Vulkan render pass.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan render pass.");
	}
}

void VulkanRenderPass::reconstruct(const RenderPassCreateInfo& aInfo)
{
	_destroy();
	construct(aInfo);
}

VkRenderPass VulkanRenderPass::getHandle() const
{
	return mPass;
}

void VulkanRenderPass::_destroy() const
{
	const auto context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyRenderPass(context->getDevice(), mPass, nullptr);
}
