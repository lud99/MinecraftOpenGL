#include "InputHandler.h"

#include <GLFW/glfw3.h>

void InputHandler::Init(GLFWwindow* window)
{
	m_Window = window;

	// Initialize state
	for (int i = 0; i < MouseButtonCount; i++)
		MouseButtonStates[i] = MOUSE_BUTTON_RELEASED;
}

bool InputHandler::IsKeyPressed(int key)
{
	return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

bool InputHandler::IsMouseButtonPressed(int button)
{
	return MouseButtonStates[button] == MOUSE_BUTTON_PRESSED;
}

bool InputHandler::isMouseButtonHeld(int button)
{
	return MouseButtonStates[button] == MOUSE_BUTTON_PRESSED;
}

void InputHandler::UpdateMouseButtonState(int button, int action, int mods)
{
	MouseButtonStates[button] = action;
}

void InputHandler::SetWindow(GLFWwindow* window) { m_Window = window; }
GLFWwindow* InputHandler::GetWindow() { return m_Window; }

int InputHandler::MouseButtonStates[InputHandler::MouseButtonCount];
GLFWwindow* InputHandler::m_Window = nullptr;