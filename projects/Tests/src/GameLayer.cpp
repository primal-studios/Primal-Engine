#include "GameLayer.h"

#include <glad/glad.h>

GameLayer::GameLayer()
{

}

GameLayer::~GameLayer()
{

}

void GameLayer::onAttach()
{

}

void GameLayer::onUpdate()
{

}

void GameLayer::onRender()
{
	glClearColor(1, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GameLayer::onDetach()
{

}

void GameLayer::onEvent(Event& aEvent)
{

}

