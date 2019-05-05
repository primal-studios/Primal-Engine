#include "application/Application.h"

#include "core/Log.h"

Application* Application::sInstance;

Application::Application()
{
	Log::construct();

	sInstance = this;

	mWindow = Window::create();
	mWindow->setEventCallback(BIND_EVENT_FUNCTION(Application::onEvent));
}

Application::~Application()
{
	delete mWindow;
	sInstance = nullptr;
}

void Application::run() const
{
	while(mRunning)
	{
		if(Input::isKeyPressed(KEY_ESCAPE))
		{
			mWindow->close();
		}

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