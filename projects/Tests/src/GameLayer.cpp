#include "GameLayer.h"

#include <assets/ShaderAsset.h>
#include <assets/TextureAsset.h>
#include <core/PrimalCast.h>
#include <ecs/EntityManager.h>
#include <components/MeshContainerComponent.h>
#include <components/MeshRenderComponent.h>
#include <graphics/Material.h>
#include <graphics/MaterialManager.h>
#include <graphics/vk/VulkanPipelineLayout.h>

#include <stb/stb_image.h>

struct UBO
{
	Matrix4f model;
	Matrix4f view;
	Matrix4f proj;
	Matrix4f mvp;
};

GameLayer::GameLayer()
{

}

GameLayer::~GameLayer()
{

}

void GameLayer::onAttach()
{
	Entity* entity = EntityManager::instance().create("CacEntity");

	entity->transform->position = Vector3f(20, 10, 20);

	auto meshAsset = AssetManager::instance().load<MeshAsset>("MeshName", "data/models/cube.glb");
	MeshContainerComponent* comp = entity->addComponent<MeshContainerComponent>(meshAsset->getMesh());

	mShaderAsset = AssetManager::instance().load<ShaderAsset>("testShader", "data/effects/default.json");
	mGraphicsPipeline = mShaderAsset->getPipeline();
	mLayout = primal_cast<VulkanPipelineLayout*>(mGraphicsPipeline->getCreateInfo().layout);

	std::vector<DescriptorPoolSize> poolSizes;
	DescriptorPoolSize uniformBufferSize{};
	uniformBufferSize.type = EDescriptorType::UNIFORM_BUFFER_DYNAMIC;
	uniformBufferSize.count = 4;

	DescriptorPoolSize combinedSamplerSize{};
	combinedSamplerSize.type = EDescriptorType::COMBINED_IMAGE_SAMPLER;
	combinedSamplerSize.count = 2;

	poolSizes.push_back(combinedSamplerSize);

	DescriptorPoolCreateInfo createInfo;
	createInfo.flags = 0;
	createInfo.maxSets = 4;
	createInfo.poolSizes = poolSizes;

	mDescPool = new DescriptorSetPool(2, createInfo);

	UniformBufferObjectElement* modl = new UniformBufferObjectElement{
		"model",
		EUniformBufferObjectElementType::UBO_TYPE_MAT4,
		0,
		sizeof(UBO::model)
	};

	UniformBufferObjectElement* view = new UniformBufferObjectElement{
		"view",
		EUniformBufferObjectElementType::UBO_TYPE_MAT4,
		0,
		sizeof(UBO::view)
	};

	UniformBufferObjectElement* proj = new UniformBufferObjectElement{
		"proj",
		EUniformBufferObjectElementType::UBO_TYPE_MAT4,
		64,
		sizeof(UBO::proj)
	};

	std::vector<UniformBufferObjectElement*> elements = { modl };

	UniformBufferCreateInfo uniformBufferCreateInfo = {};
	uniformBufferCreateInfo.flags = 0;
	uniformBufferCreateInfo.sharingMode = SHARING_MODE_EXCLUSIVE;
	uniformBufferCreateInfo.size = 65536;
	uniformBufferCreateInfo.usage = EBufferUsageFlagBits::BUFFER_USAGE_UNIFORM_BUFFER;

	mUboPool = new UniformBufferPool(65536 / sizeof(UBO), sizeof(UBO), 0, uniformBufferCreateInfo, elements);

	const auto texAsset = AssetManager::instance().load<TextureAsset>("test", "data/textures/Shawn.json", STBI_rgb_alpha);

	MaterialCreateInfo materialCreateInfo;
	materialCreateInfo.layouts = { mUboPool };
	materialCreateInfo.pipeline = mGraphicsPipeline;
	materialCreateInfo.pool = mDescPool;
	materialCreateInfo.textures = { { "albedo", texAsset->getTexture() } };
	MaterialInstance* materialInstance = MaterialManager::instance().createMaterial(materialCreateInfo)->createInstance();

	MeshRenderComponent* renderComp = entity->addComponent<MeshRenderComponent>(materialInstance);
}

void GameLayer::onUpdate()
{

}

void GameLayer::onRender()
{

}

void GameLayer::onDetach()
{
	MaterialManager::instance().reset();
	delete mUboPool;
	delete mDescPool;
	AssetManager::instance().unloadAll();
}

void GameLayer::onEvent(Event& aEvent)
{

}

