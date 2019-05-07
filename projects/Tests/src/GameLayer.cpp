#include "GameLayer.h"

#include "core/Property.h"

GameLayer::GameLayer()
{
	vao = nullptr;
}

GameLayer::~GameLayer()
{
	delete vao;
}

void GameLayer::onAttach()
{
	Entity* entity = EntityManager::instance().create("Cac");

	EntityManager::instance().destroy(entity);

	const float data[] = 
	{
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f,  1.0f, 0.0f
	};

	uint32_t indices[] =
	{
		0, 1, 2 // 0
	};

	VertexBuffer* vbo = new VertexBuffer(data, sizeof(data));

	VertexBufferLayout layout;
	layout.push<Eigen::Vector3f>("iPosition");
	vbo->setLayout(layout);

	IndexBuffer* ibo = new IndexBuffer(indices, sizeof(indices));

	vao = new VertexArray(vbo, ibo);

	FileSystem::instance().mount("dependencies");
	auto list = FileSystem::instance().getFilesInPath("assimp", true);

	Property<float> prop = 5.0f;

	prop.getCallback([=](float& aValue)
	{
		return aValue;
	});

	prop.setCallback([=](const float aValue)
	{
		return aValue;
	});

	prop = 20.0f;

	prop *= 5.0f;

	PRIMAL_TRACE(*prop);

	int jonathan = 0;
}

void GameLayer::onUpdate()
{

}

void GameLayer::onRender()
{
	glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	vao->bind();

	glDrawElements(GL_TRIANGLES, vao->getIndexBuffer()->count(), GL_UNSIGNED_INT, nullptr);

	vao->unbind();
}

void GameLayer::onDetach()
{

}

void GameLayer::onEvent(Event& aEvent)
{

}

