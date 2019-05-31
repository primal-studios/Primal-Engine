#include "graphics/vk/VulkanGraphicsPipeline.h"
#include "graphics/vk/VulkanShaderStage.h"
#include "core/PrimalCast.h"
#include "graphics/vk/VulkanPipelineLayout.h"
#include "graphics/vk/VulkanGraphicsContext.h"

VulkanGraphicsPipeline::VulkanGraphicsPipeline(IGraphicsContext* aContext) : IGraphicsPipeline(aContext)
{
	mPipeline = nullptr;
}

VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	VulkanGraphicsContext* context = primal_cast<VulkanGraphicsContext*>(mContext);
	vkDestroyPipeline(context->getDevice(), mPipeline, nullptr);
}

void VulkanGraphicsPipeline::construct(const GraphicsPipelineCreateInfo& aInfo)
{
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
	vertexState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexState.flags = aInfo.vertexState.flags;

	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	for(const auto& a : aInfo.vertexState.attributeDescriptions)
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

	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	for(const auto& b : aInfo.vertexState.bindingDescriptions)
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

	VkPipelineInputAssemblyStateCreateInfo assemblyState = {};
	assemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyState.flags = aInfo.assemblyState.flags;
	assemblyState.primitiveRestartEnable = aInfo.assemblyState.primitiveRestartEnable;
	assemblyState.topology = static_cast<VkPrimitiveTopology>(aInfo.assemblyState.topology);

	createInfo.pInputAssemblyState = &assemblyState;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.flags = aInfo.viewportState.flags;

	std::vector<VkRect2D> scissors;
	for(const auto& s : aInfo.viewportState.rectangles)
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

	std::vector<VkViewport> viewports;
	for(const auto& v : aInfo.viewportState.viewports)
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

	VkPipelineRasterizationStateCreateInfo rasterizationState = {};
	rasterizationState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationState.cullMode = static_cast<VkCullModeFlags>(aInfo.rasterizationState.cullMode);
	rasterizationState.depthBiasClamp = aInfo.rasterizationState.depthBiasClamp;
	rasterizationState.depthBiasConstantFactor = aInfo.rasterizationState.depthBiasConstantFactor;
	rasterizationState.depthBiasEnable = aInfo.rasterizationState.depthBiasEnable;
	rasterizationState.depthBiasSlopeFactor = aInfo.rasterizationState.depthBiasSlopeFactor;
	rasterizationState.depthClampEnable = aInfo.rasterizationState.depthClampEnable;
	rasterizationState.flags = aInfo.rasterizationState.flags;
	rasterizationState.frontFace = static_cast<VkFrontFace>(aInfo.rasterizationState.frontFace);
	rasterizationState.lineWidth = aInfo.rasterizationState.lineWidth;
	rasterizationState.polygonMode = static_cast<VkPolygonMode>(aInfo.rasterizationState.polygonMode);
	rasterizationState.rasterizerDiscardEnable = aInfo.rasterizationState.rasterizerDiscardEnable;

	createInfo.pRasterizationState = &rasterizationState;

	VkPipelineMultisampleStateCreateInfo multisampleState = {};
	multisampleState.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleState.alphaToCoverageEnable = aInfo.multisampleState.alphaToCoverageEnable;
	multisampleState.alphaToOneEnable = aInfo.multisampleState.alphaToOneEnable;
	multisampleState.flags = aInfo.multisampleState.flags;
	multisampleState.minSampleShading = aInfo.multisampleState.minSampleShading;
	multisampleState.pSampleMask = aInfo.multisampleState.sampleMask;
	multisampleState.rasterizationSamples = static_cast<VkSampleCountFlagBits>(aInfo.multisampleState.rasterizationSamples);
	multisampleState.sampleShadingEnable = aInfo.multisampleState.sampleShadingEnable;

	createInfo.pMultisampleState = &multisampleState;

	VkPipelineDepthStencilStateCreateInfo depthStencilState = {};
	depthStencilState.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;

	// TODO(Nick): Do this shit please
	createInfo.pDepthStencilState = nullptr;

	VkPipelineColorBlendStateCreateInfo colorBlendState = {};
	colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendState.logicOpEnable = aInfo.colorBlendState.logicOpEnable;
	colorBlendState.logicOp = static_cast<VkLogicOp>(aInfo.colorBlendState.logicOp);
	colorBlendState.attachmentCount = static_cast<uint32_t>(aInfo.colorBlendState.attachments.size());
	colorBlendState.flags = aInfo.colorBlendState.flags;

	std::vector<VkPipelineColorBlendAttachmentState> attachments;
	for(const auto& a : aInfo.colorBlendState.attachments)
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

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.flags = aInfo.dynamicState.flags;
	dynamicState.dynamicStateCount = static_cast<uint32_t>(aInfo.dynamicState.dynamicStates.size());

	std::vector<VkDynamicState> dynamicStates;
	for(const auto& d : aInfo.dynamicState.dynamicStates)
	{
		VkDynamicState state = static_cast<VkDynamicState>(d);
		dynamicStates.push_back(state);
	}

	dynamicState.pDynamicStates = dynamicStates.data();

	createInfo.pDynamicState = &dynamicState;

	VulkanPipelineLayout* layout = primal_cast<VulkanPipelineLayout*>(aInfo.layout);
	createInfo.layout = layout->getHandle();

	VulkanGraphicsPipeline* basePipeline = primal_cast<VulkanGraphicsPipeline*>(aInfo.basePipelineHandle);
	createInfo.basePipelineHandle = basePipeline->getHandle();
	createInfo.basePipelineIndex = aInfo.basePipelineIndex;

	const VkResult result = vkCreateGraphicsPipelines(context->getDevice(), nullptr, 1, &createInfo, nullptr, &mPipeline);

}

VkPipeline VulkanGraphicsPipeline::getHandle() const
{
	return mPipeline;
}
