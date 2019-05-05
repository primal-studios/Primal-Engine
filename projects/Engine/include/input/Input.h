#ifndef input_h__
#define input_h__

#include <cstdint>

#include "input/KeyCodes.h"

enum class EKeyState : uint32_t
{
	None,
	Pressed,
	Down,
	Up
};

class Input
{
	friend class Window;
	friend class Application;

	public:
		static bool isKeyDown(const uint32_t aKeyCode);
		static bool isKeyPressed(const uint32_t aKeyCode);
		static bool isKeyUp(const uint32_t aKeyCode);

	private:
		static void _poll();

		static EKeyState sKeyStates[KEY_LAST];
		static EKeyState sKeyLastStates[KEY_LAST];

		static void _setKeyPressed(const uint32_t aKeyCode);
		static void _setKeyReleased(const uint32_t aKeyCode);

		static void _setMousePosition(const double aX, const double aY);
};

#endif // input_h__