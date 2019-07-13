#include "core/Log.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanGraphicsContext.h"
#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/vk/VulkanShaderStage.h"
#include "graphics/vk/VulkanRenderPass.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline(IGraphicsContext* aContext) 
	: IGraphicsPipeline(aContext)
{
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	_destroy();
}

void VulkanGraphicsPipeline::construct(const GraphicsPipelineCreateInfo& aInfo)
{
	mCreateInfo = aInfo;

	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);

	VkGraphicsPipelineCreateInfo createInfo = {};

	createInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = static_cast<uint32_t>(aInfo.stages.size());

	std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;
	for(const auto& s : aInfo.stages)
	{
		VulkanShaderStage* vkStage = primal_cast<VulkanShaderStage*>(s);
		shaderStageCreateInfos.push_back(vkStage->getStage());
	}

	createInfo.pStages = shaderStageCreateInfos.data();

	VkPipelineVertexInputStateCreateInfo vertexState = {};
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	std::vector<VkVertexInputBindingDescription> bindingDescriptions;

	if (aInfo.vertexState)
	{
		vertexState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexState.flags = aInfo.vertexState->flags;

		for (const auto& a : aInfo.vertexState->attributeDescriptions)
		{
			VkVertexInputAttributeDescription desc = {};
			desc.binding = a.binding;
			desc.format = static_cast<VkFormat>(a.format);
			desc.location = a.location;
			desc.offset = a.offset;

			attributeDescriptions.push_back(desc);
		}

		vertexState.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexState.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	
		for (const auto& b : aInfo.vertexState->bindingDescriptions)
		{
			VkVertexInputBindingDescription desc = {};
			desc.binding = b.binding;
			desc.inputRate = static_cast<VkVertexInputRate>(b.rate);
			desc.stride = b.stride;

			bindingDescriptions.push_back(desc);
		}

		vertexState.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexState.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());

		createInfo.pVertexInputState = &vertexState;
	}
	else
	{
		createInfo.pVertexInputState = nullptr;
	}

	VkPipelineInputAssemblyStateCreateInfo assemblyState = {};

	if (aInfo.assemblyState)
	{
		assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyState.flags = aInfo.assemblyState->flags;
		assemblyState.primitiveRestartEnable = aInfo.assemblyState->primitiveRestartEnable;
		assemblyState.topology = static_cast<VkPrimitiveTopology>(aInfo.assemblyState->topology);
		createInfo.pInputAssemblyState = &assemblyState;
	}
	else
	{
		createInfo.pInputAssemblyState = nullptr;
	}

	VkPipelineViewportStateCreateInfo viewportState = {};
	std::vector<VkRect2D> scissors;
	std::vector<VkViewport> viewports;

	if (aInfo.viewportState)
	{
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.flags = aInfo.viewportState->flags;

		for (const auto& s : aInfo.viewportState->rectangles)
		{
			VkRect2D rect;
			rect.offset.x = s.x;
			rect.offset.y = s.y;

			rect.extent.width = s.z;
			rect.extent.height = s.w;

			scissors.push_back(rect);
		}

		viewportState.pScissors = scissors.data();
		viewportState.scissorCount = static_cast<uint32_t>(scissors.size());

		for (const auto& v : aInfo.viewportState->viewports)
		{
			VkViewport viewport = {};
			viewport.x = v.x;
			viewport.y = v.y;
			viewport.width = v.width;
			viewport.height = v.height;
			viewport.minDepth = v.minDepth;
			viewport.maxDepth = v.maxDepth;

			viewports.push_back(viewport);
		}

		viewportState.pViewports = viewports.data();
		viewportState.viewportCount = static_cast<uint32_t>(viewports.size());

		createInfo.pViewportState = &viewportState;
	}
	else
	{
		createInfo.pViewportState = nullptr;
	}

	VkPipelineRasterizationStateCreateInfo rasterizationState = {};

	if (aInfo.rasterizationState)
	{
		rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationState.cullMode = static_cast<VkCullModeFlags>(aInfo.rasterizationState->cullMode);
		rasterizationState.depthBiasClamp = aInfo.rasterizationState->depthBiasClamp;
		rasterizationState.depthBiasConstantFactor = aInfo.rasterizationState->depthBiasConstantFactor;
		rasterizationState.depthBiasEnable = aInfo.rasterizationState->depthBiasEnable;
		rasterizationState.depthBiasSlopeFactor = aInfo.rasterizationState->depthBiasSlopeFactor;
		rasterizationState.depthClampEnable = aInfo.rasterizationState->depthClampEnable;
		rasterizationState.flags = aInfo.rasterizationState->flags;
		rasterizationState.frontFace = static_cast<VkFrontFace>(aInfo.rasterizationState->frontFace);
		rasterizationState.lineWidth = aInfo.rasterizationState->lineWidth;
		rasterizationState.polygonMode = static_cast<VkPolygonMode>(aInfo.rasterizationState->polygonMode);
		rasterizationState.rasterizerDiscardEnable = aInfo.rasterizationState->rasterizerDiscardEnable;

		createInfo.pRasterizationState = &rasterizationState;
	}
	else
	{
		createInfo.pRasterizationState = nullptr;
	}

	VkPipelineMultisampleStateCreateInfo multisampleState = {};

	if (aInfo.multisampleState)
	{
		multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleState.alphaToCoverageEnable = aInfo.multisampleState->alphaToCoverageEnable;
		multisampleState.alphaToOneEnable = aInfo.multisampleState->alphaToOneEnable;
		multisampleState.flags = aInfo.multisampleState->flags;
		multisampleState.minSampleShading = aInfo.multisampleState->minSampleShading;
		multisampleState.pSampleMask = aInfo.multisampleState->sampleMask;
		multisampleState.rasterizationSamples = static_cast<VkSampleCountFlagBits>(aInfo.multisampleState->rasterizationSamples);
		multisampleState.sampleShadingEnable = aInfo.multisampleState->sampleShadingEnable;

		createInfo.pMultisampleState = &multisampleState;
	}
	else
	{
		createInfo.pMultisampleState = nullptr;
	}

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
	VkStencilOpState front = {};
	VkStencilOpState back = {};

	if (aInfo.depthStencilState)
	{
		depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencilState.flags = aInfo.depthStencilState->flags;
		depthStencilState.depthTestEnable = aInfo.depthStencilState->depthTestEnable;
		depthStencilState.depthWriteEnable = aInfo.depthStencilState->depthWriteEnable;
		depthStencilState.depthCompareOp = static_cast<VkCompareOp>(aInfo.depthStencilState->depthCompareOp);
		depthStencilState.depthBoundsTestEnable = aInfo.depthStencilState->depthBoundsTestEnable;
		depthStencilState.stencilTestEnable = aInfo.depthStencilState->stencilTestEnable;
		depthStencilState.minDepthBounds = aInfo.depthStencilState->minDepthBounds;
		depthStencilState.maxDepthBounds = aInfo.depthStencilState->maxDepthBounds;

		front.failOp = static_cast<VkStencilOp>(aInfo.depthStencilState->front.failOp);
		front.passOp = static_cast<VkStencilOp>(aInfo.depthStencilState->front.passOp);
		front.depthFailOp = static_cast<VkStencilOp>(aInfo.depthStencilState->front.depthFailOp);
		front.compareOp = static_cast<VkCompareOp>(aInfo.depthStencilState->front.compareOp);
		front.compareMask = aInfo.depthStencilState->front.compareMask;
		front.writeMask = aInfo.depthStencilState->front.writeMask;
		front.reference = aInfo.depthStencilState->front.reference;

		back.failOp = static_cast<VkStencilOp>(aInfo.depthStencilState->back.failOp);
		back.passOp = static_cast<VkStencilOp>(aInfo.depthStencilState->back.passOp);
		back.depthFailOp = static_cast<VkStencilOp>(aInfo.depthStencilState->back.depthFailOp);
		back.compareOp = static_cast<VkCompareOp>(aInfo.depthStencilState->back.compareOp);
		back.compareMask = aInfo.depthStencilState->back.compareMask;
		back.writeMask = aInfo.depthStencilState->back.writeMask;
		back.reference = aInfo.depthStencilState->back.reference;

		depthStencilState.front = front;
		depthStencilState.back = back;

		createInfo.pDepthStencilState = &depthStencilState;
	}
	else
	{
		createInfo.pDepthStencilState = nullptr;
	}

	VkPipelineColorBlendStateCreateInfo colorBlendState = {};
	std::vector<VkPipelineColorBlendAttachmentState> attachments;

	if (aInfo.colorBlendState)
	{
		colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendState.logicOpEnable = aInfo.colorBlendState->logicOpEnable;
		colorBlendState.logicOp = static_cast<VkLogicOp>(aInfo.colorBlendState->logicOp);
		colorBlendState.attachmentCount = static_cast<uint32_t>(aInfo.colorBlendState->attachments.size());
		colorBlendState.flags = aInfo.colorBlendState->flags;

		for (const auto& a : aInfo.colorBlendState->attachments)
		{
			VkPipelineColorBlendAttachmentState state = {};
			state.alphaBlendOp = static_cast<VkBlendOp>(a.alphaBlendOp);
			state.blendEnable = a.blendEnable;
			state.colorBlendOp = static_cast<VkBlendOp>(a.colorBlendOp);
			state.colorWriteMask = a.colorWriteMask;
			state.dstAlphaBlendFactor = static_cast<VkBlendFactor>(a.dstAlphaBlendFactor);
			state.srcAlphaBlendFactor = static_cast<VkBlendFactor>(a.srcAlphaBlendFactor);
			state.dstColorBlendFactor = static_cast<VkBlendFactor>(a.dstColorBlendFactor);
			state.srcColorBlendFactor = static_cast<VkBlendFactor>(a.srcColorBlendFactor);

			attachments.push_back(state);
		}

		colorBlendState.pAttachments = attachments.data();

		createInfo.pColorBlendState = &colorBlendState;
	}
	else
	{
		createInfo.pColorBlendState = nullptr;
	}
	
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	std::vector<VkDynamicState> dynamicStates;

	if (aInfo.dynamicState)
	{
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.flags = aInfo.dynamicState->flags;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(aInfo.dynamicState->dynamicStates.size());

		for (const auto& d : aInfo.dynamicState->dynamicStates)
		{
			VkDynamicState state = static_cast<VkDynamicState>(d);
			dynamicStates.push_back(state);
		}

		dynamicState.pDynamicStates = dynamicStates.data();

		createInfo.pDynamicState = &dynamicState;
	}
	else
	{
		createInfo.pDynamicState = nullptr;
	}

	if (aInfo.layout != nullptr)
	{
		VulkanPipelineLayout* layout = primal_cast<VulkanPipelineLayout*>(aInfo.layout);
		createInfo.layout = layout->getHandle();
	}

	if (aInfo.basePipelineHandle != nullptr)
	{
		VulkanGraphicsPipeline* basePipeline = primal_cast<VulkanGraphicsPipeline*>(aInfo.basePipelineHandle);
		createInfo.basePipelineHandle = basePipeline->getHandle();
	}

	createInfo.basePipelineIndex = aInfo.basePipelineIndex;

	if(aInfo.renderPass != nullptr)
	{
		VulkanRenderPass* renderPass = primal_cast<VulkanRenderPass*>(aInfo.renderPass);
		createInfo.renderPass = renderPass->getHandle();
		createInfo.subpass = aInfo.subPass;
	}
	else
	{
		createInfo.renderPass = nullptr;
		createInfo.subpass = 0;
	}

	const VkResult result = vkCreateGraphicsPipelines(context->getDevice(), nullptr, 1, &createInfo, nullptr, &mPipeline);
	if (result != VK_SUCCESS)
	{
		PRIMAL_INTERNAL_CRITICAL("Failed to construct Vulkan graphics pipeline.");
	}
	else
	{
		PRIMAL_INTERNAL_INFO("Successfully created Vulkan graphics pipeline.");
	}
}

void VulkanGraphicsPipeline::reconstruct(const GraphicsPipelineCreateInfo& aInfo)
{
	destroy();
	construct(aInfo);
}

void VulkanGraphicsPipeline::destroy()
{
	_destroy();
}

GraphicsPipelineCreateInfo& VulkanGraphicsPipeline::getCreateInfo()
{
	return mCreateInfo;
}

VkPipeline VulkanGraphicsPipeline::getHandle() const
{
	return mPipeline;
}

VulkanPipelineLayout* VulkanGraphicsPipeline::getLayout() const
{
	return primal_cast<VulkanPipelineLayout*>(mCreateInfo.layout);
}

void VulkanGraphicsPipeline::_destroy() const
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyPipeline(context->getDevice(), mPipeline, nullptr);
}
