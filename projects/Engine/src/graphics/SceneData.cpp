#include "graphics/SceneData.h"
#include "graphics/GraphicsFactory.h"

SceneData::SceneData(const SceneDataCreateInfo& aInfo)
	: mInfo(aInfo)
{
	size_t sz = 0;
	for (const auto& element : aInfo.elements)
	{
		sz += element->size;
		mOffsets.insert({ element->name, element->offset });
	}
	mBackingSz = sz;
	mCpuBacking = malloc(sz);
	UniformBufferCreateInfo createInfo = {};
	createInfo.flags = 0;
	createInfo.sharingMode = SHARING_MODE_EXCLUSIVE;
	createInfo.size = sz;
	createInfo.usage = BUFFER_USAGE_UNIFORM_BUFFER;

	mUboPool = new UniformBufferPool(1, static_cast<uint32_t>(sz), 0, createInfo, aInfo.elements);
	mObj = mUboPool->acquire();

	mDescSetPool = GraphicsFactory::instance().createDescriptorPool();
	mSet = GraphicsFactory::instance().createDescriptorSet();

	DescriptorPoolCreateInfo descPoolCreateInfo = {};
	descPoolCreateInfo.flags = 0;
	descPoolCreateInfo.maxSets = 2;

	DescriptorPoolSize uniformBufferSize = {};
	uniformBufferSize.type = EDescriptorType::UNIFORM_BUFFER;
	uniformBufferSize.count = 2;

	descPoolCreateInfo.poolSizes.push_back(uniformBufferSize);

	mDescSetPool->construct(descPoolCreateInfo);

	DescriptorSetCreateInfo info = {};
	info.pool = mDescSetPool;

	IDescriptorSetLayout* layout = GraphicsFactory::instance().createDescriptorSetLayout();
	DescriptorSetLayoutCreateInfo setLayoutCreateInfo = {};
	DescriptorSetLayoutBinding binding = {};
	binding.binding = 0;
	binding.descriptorCount = 1;
	binding.descriptorType = EDescriptorType::UNIFORM_BUFFER;
	binding.shaderStageFlags = SHADER_STAGE_ALL_GRAPHICS;
	
	setLayoutCreateInfo.layoutBindings.push_back(binding);
	setLayoutCreateInfo.flags = 0;
	layout->construct(setLayoutCreateInfo);
	
	info.setLayouts.push_back(layout);
	info.setLayouts.push_back(layout);

	mSet->construct(info);
	mSetLayout = layout;
}

SceneData::~SceneData()
{
	free(mCpuBacking);
	delete mSet;
	delete mDescSetPool;
	delete mUboPool;
	delete mSetLayout;
}
