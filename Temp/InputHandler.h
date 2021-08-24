#pragma once

#define MOUSE_BUTTON_RELEASED 0
#define MOUSE_BUTTON_PRESSED 1
#define MOUSE_BUTTON_HOLD 2

struct GLFWwindow;

class InputHandler
{
public:
	static void Init(GLFWwindow* window);

	static void Clear();

	static bool IsKeyHeld(int key);
	static bool IsKeyPressed(int key);
	static void UpdateKeyState(int key, int action, int mods);

	static bool IsMouseButtonPressed(int button);
	static bool IsMouseButtonHeld(int button);
	static void UpdateMouseButtonState(int keys, int action, int mods);

	//static void UpdateKeyStates();

	
	//static void UpdateMouseButtonStates();

	static void SetWindow(GLFWwindow* window);
	static GLFWwindow* GetWindow();

public:
	const static int MouseButtonCount = 10;
	const static int KeyCount = 256 * 256;

	static int MouseButtonStates[MouseButtonCount];
	static int PreviousMouseButtonStates[MouseButtonCount];

	static int KeyStates[KeyCount];
	static int PreviousKeyStates[KeyCount];

private:
	static GLFWwindow* m_Window;
};