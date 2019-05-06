#include "input/Input.h"

EKeyState Input::sKeyStates[KEY_LAST];
EKeyState Input::sKeyLastStates[KEY_LAST];

EKeyState Input::sMouseStates[MOUSE_BUTTON_LAST];
EKeyState Input::sMouseLastStates[MOUSE_BUTTON_LAST];

Eigen::Vector2d Input::sMousePosition;
Eigen::Vector2f Input::sMouseScroll;

bool Input::isKeyDown(const uint32_t aKeyCode)
{
	return sKeyStates[aKeyCode] == EKeyState::Down || sKeyStates[aKeyCode] == EKeyState::Pressed;
}

bool Input::isKeyPressed(const uint32_t aKeyCode)
{
	return sKeyStates[aKeyCode] == EKeyState::Pressed;
}

bool Input::isKeyUp(const uint32_t aKeyCode)
{
	return sKeyStates[aKeyCode] == EKeyState::Up;
}

bool Input::isMouseButtonDown(const uint32_t aMouseCode)
{
	return sMouseStates[aMouseCode] == EKeyState::Down || sMouseStates[aMouseCode] == EKeyState::Pressed;
}

bool Input::isMouseButtonPressed(const uint32_t aMouseCode)
{
	return sMouseStates[aMouseCode] == EKeyState::Pressed;
}

bool Input::isMouseButtonUp(const uint32_t aMouseCode)
{
	return sMouseStates[aMouseCode] == EKeyState::Up;
}

const Eigen::Vector2d& Input::getMousePosition()
{
	return sMousePosition;
}

const Eigen::Vector2f& Input::getScroll()
{
	return sMouseScroll;
}

void Input::_poll()
{
	for (uint32_t i = 0; i < KEY_LAST; i++)
	{
		if (sKeyStates[i] == EKeyState::Pressed && sKeyLastStates[i] == EKeyState::Pressed)
		{
			sKeyStates[i] = EKeyState::Down;
		}

		if (sKeyStates[i] == EKeyState::Up && sKeyLastStates[i] == EKeyState::Up)
		{
			sKeyStates[i] = EKeyState::None;
		}

		sKeyLastStates[i] = sKeyStates[i];
	}

	for (uint32_t i = 0; i < MOUSE_BUTTON_LAST; i++)
	{
		if (sMouseStates[i] == EKeyState::Pressed && sMouseLastStates[i] == EKeyState::Pressed)
		{
			sMouseStates[i] = EKeyState::Down;
		}

		if (sMouseStates[i] == EKeyState::Up && sMouseLastStates[i] == EKeyState::Up)
		{
			sMouseStates[i] = EKeyState::None;
		}

		sMouseLastStates[i] = sMouseStates[i];
	}
}

void Input::_setKeyPressed(const uint32_t aKeyCode)
{
	if (aKeyCode < KEY_LAST)
	{
		if (sKeyStates[aKeyCode] == EKeyState::Pressed || sKeyStates[aKeyCode] == EKeyState::Down)
			return;

		sKeyStates[aKeyCode] = EKeyState::Pressed;
	}
}

void Input::_setKeyReleased(const uint32_t aKeyCode)
{
	if (aKeyCode < KEY_LAST)
	{
		sKeyStates[aKeyCode] = EKeyState::Up;
	}
}

void Input::_setMousePressed(const uint32_t aMouseCode)
{
	if(aMouseCode < MOUSE_BUTTON_LAST)
	{
		if (sMouseStates[aMouseCode] == EKeyState::Pressed || sMouseStates[aMouseCode] == EKeyState::Down)
			return;

		sMouseStates[aMouseCode] = EKeyState::Pressed;
	}
}

void Input::_setMouseReleased(const uint32_t aMouseCode)
{
	if(aMouseCode < MOUSE_BUTTON_LAST)
	{
		sMouseStates[aMouseCode] = EKeyState::Up;
	}
}

void Input::_setMouseScroll(const float aXOffset, const float aYOffset)
{
	sMouseScroll = Eigen::Vector2f(aXOffset, aYOffset);
}

void Input::_setMousePosition(const double aX, const double aY)
{
	sMousePosition = Eigen::Vector2d(aX, aY);
}
