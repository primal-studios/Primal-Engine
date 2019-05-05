#include "input/Input.h"

EKeyState Input::sKeyStates[KEY_LAST];
EKeyState Input::sKeyLastStates[KEY_LAST];

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

void Input::_setMousePosition(const double aX, const double aY)
{

}
