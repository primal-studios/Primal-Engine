#ifndef input_h__
#define input_h__

#include <cstdint>
#include <Eigen/Eigen>

#include "input/KeyCodes.h"
#include "input/MouseCodes.h"

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

		static bool isMouseButtonDown(const uint32_t aMouseCode);
		static bool isMouseButtonPressed(const uint32_t aMouseCode);
		static bool isMouseButtonUp(const uint32_t aMouseCode);

		static const Eigen::Vector2d& getMousePosition();
		static const Eigen::Vector2f& getScroll();

	private:
		static void _poll();

		static EKeyState sKeyStates[KEY_LAST];
		static EKeyState sKeyLastStates[KEY_LAST];

		static EKeyState sMouseStates[MOUSE_BUTTON_LAST];
		static EKeyState sMouseLastStates[MOUSE_BUTTON_LAST];

		static Eigen::Vector2d sMousePosition;
		static Eigen::Vector2f sMouseScroll;

		static void _setKeyPressed(const uint32_t aKeyCode);
		static void _setKeyReleased(const uint32_t aKeyCode);

		static void _setMousePressed(const uint32_t aMouseCode);
		static void _setMouseReleased(const uint32_t aMouseCode);

		static void _setMouseScroll(const float aXOffset, const float aYOffset);

		static void _setMousePosition(const double aX, const double aY);
};

#endif // input_h__