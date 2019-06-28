#include "application/Application.h"

#include "core/Log.h"
#include "input/Input.h"
#include "ecs/SystemManager.h"
#include "systems/RenderSystem.h"
#include "physics/PhysicsSystem.h"
#include "application/ApplicationLayer.h"
#include "systems/VulkanRenderSystem.h"

Application* Application::sInstance;

Application::Application()
{
	Log::construct();

	sInstance = this;

	mWindow = Window::create();
	mWindow->setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));

	SystemManager::instance().addSystem<RenderSystem>(mWindow);
	SystemManager::instance().addSystem<PhysicsSystem>();
	SystemManager::instance().configure();
}

Application::~Application()
{
	SystemManager::instance().removeSystem<RenderSystem>();
	SystemManager::instance().removeSystem<PhysicsSystem>();
	delete mWindow;
	sInstance = nullptr;
}

void Application::run() const
{
	while(mRunning)
	{
		Input::_poll();

		if(Input::isKeyPressed(KEY_ESCAPE))
		{
			mWindow->close();
		}

		SystemManager::instance().update();
		SystemManager::instance().fixedUpdate();

		SystemManager::instance().render();

		for (ApplicationLayer* layer : mLayerStack)
			layer->onUpdate();

		for (ApplicationLayer* layer : mLayerStack)
			layer->onRender();

		mWindow->refresh();
		mWindow->pollEvents();
	}
}

void Application::onEvent(Event& aEvent)
{
	EventDispatcher dispatcher(aEvent);
	dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FUNCTION(Application::_onWindowClose));
	dispatcher.dispatch<KeyPressedEvent>(BIND_EVENT_FUNCTION(Application::_onKeyPressed));
	dispatcher.dispatch<KeyReleasedEvent>(BIND_EVENT_FUNCTION(Application::_onKeyReleased));
	dispatcher.dispatch<MouseMovedEvent>(BIND_EVENT_FUNCTION(Application::_onMouseMoved));
	dispatcher.dispatch<MouseButtonPressedEvent>(BIND_EVENT_FUNCTION(Application::_onMousePressed));
	dispatcher.dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FUNCTION(Application::_onMouseReleased));
	dispatcher.dispatch<MouseScrolledEvent>(BIND_EVENT_FUNCTION(Application::_onMouseScrolled));

	SystemManager::instance().dispatchEvent(aEvent);

	for (auto it = mLayerStack.end(); it != mLayerStack.begin(); )
	{
		(*--it)->onEvent(aEvent);
		if (aEvent.isHandled())
			break;
	}
}

void Application::pushLayer(ApplicationLayer* aLayer)
{
	mLayerStack.pushLayer(aLayer);
	aLayer->onAttach();
}

void Application::popLayer(ApplicationLayer* aLayer)
{
	mLayerStack.popLayer(aLayer);
	aLayer->onDetach();
}

void Application::pushOverlay(ApplicationLayer* aOverlay)
{
	mLayerStack.pushOverlay(aOverlay);
	aOverlay->onAttach();
}

void Application::popOverlay(ApplicationLayer* aOverlay)
{
	mLayerStack.popOverlay(aOverlay);
	aOverlay->onDetach();
}

bool Application::_onWindowClose(WindowCloseEvent& aEvent)
{
	mRunning = false;
	return true;
}

bool Application::_onKeyPressed(KeyPressedEvent& aEvent) const
{
	Input::_setKeyPressed(aEvent.getKeyCode());
	return false;
}

bool Application::_onKeyReleased(KeyReleasedEvent& aEvent) const
{
	Input::_setKeyReleased(aEvent.getKeyCode());
	return false;
}

bool Application::_onMouseMoved(MouseMovedEvent& aEvent) const
{
	Input::_setMousePosition(aEvent.getX(), aEvent.getY());
	return false;
}

bool Application::_onMousePressed(MouseButtonPressedEvent& aEvent) const
{
	Input::_setMousePressed(aEvent.getMouseButton());
	return false;
}

bool Application::_onMouseReleased(MouseButtonReleasedEvent& aEvent) const
{
	Input::_setMouseReleased(aEvent.getMouseButton());
	return false;
}

bool Application::_onMouseScrolled(MouseScrolledEvent& aEvent) const
{
	Input::_setMouseScroll(aEvent.getXOffset(), aEvent.getYOffset());
	return false;
}
