#include "GameLayer.h"

GameLayer::GameLayer()
{

}

GameLayer::~GameLayer()
{

}

void GameLayer::onAttach()
{
	//SystemManager::instance().addSystem<RenderSystem>();

	//SystemManager::instance().configure();

	//const float data[] = 
	//{
	//	-1.0f, -1.0f, 0.0f,
	//	1.0f, -1.0f, 0.0f,
	//	0.0f,  1.0f, 0.0f
	//};

	//uint32_t indices[] =
	//{
	//	0, 1, 2 // 0
	//};

	//VertexBuffer* vbo = new VertexBuffer(data, sizeof(data));

	//VertexBufferLayout layout;
	//layout.push<Eigen::Vector3f>("iPosition");
	//vbo->setLayout(layout);

	//IndexBuffer* ibo = new IndexBuffer(indices, sizeof(indices));

	//vao = new VertexArray(vbo, ibo);

	//Mesh* mesh = new Mesh(vao);

	//Entity* entity = EntityManager::instance().create("Cac");
	//entity->addComponent<MeshContainerComponent>(mesh);

	//entity->addComponent<MeshRenderComponent>();

	//FileSystem::instance().mount("dependencies");
	//auto list = FileSystem::instance().getFilesInPath("assimp", true);

	//Property<float> prop = 5.0f;

	//prop.getCallback([=](float& aValue)
	//{
	//	return aValue;
	//});

	//prop.setCallback([=](const float aValue)
	//{
	//	return aValue;
	//});

	//prop = 20.0f;

	//prop *= 5.0f;

	//PRIMAL_TRACE(*prop);

	//int jonathan = 0;
}

void GameLayer::onUpdate()
{
	//SystemManager::instance().update();
}

void GameLayer::onRender()
{
	//SystemManager::instance().render();

	//vao->bind();
	//glDrawElements(GL_TRIANGLES, vao->getIndexBuffer()->count(), GL_UNSIGNED_INT, nullptr);
	//vao->unbind();
}

void GameLayer::onDetach()
{

}

void GameLayer::onEvent(Event& aEvent)
{

}

