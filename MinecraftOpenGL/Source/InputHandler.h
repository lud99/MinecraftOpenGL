#pragma once

#define MOUSE_BUTTON_RELEASED 0;
#define MOUSE_BUTTON_PRESSED 1;
#define MOUSE_BUTTON_HOLD 2;

struct GLFWwindow;

class InputHandler
{
public:
	static void Init(GLFWwindow* window);

	static bool IsKeyPressed(int key);
	static bool IsMouseButtonPressed(int button);
	static bool isMouseButtonHeld(int button);

	static void UpdateMouseButtonState(int button, int action, int mods);

	static void SetWindow(GLFWwindow* window);
	static GLFWwindow* GetWindow();

public:
	const static int MouseButtonCount = 10;

	static int MouseButtonStates[MouseButtonCount];
	static int PreviousMouseButtonStates[MouseButtonCount];

private:
	static GLFWwindow* m_Window;
};