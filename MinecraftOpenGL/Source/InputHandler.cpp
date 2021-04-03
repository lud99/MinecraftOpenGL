#include "InputHandler.h"

#include <GLFW/glfw3.h>
#include <iostream>

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
	return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
}

void InputHandler::UpdateMouseButtonState(int button, int action, int mods)
{
	MouseButtonStates[button] = action;
}

void InputHandler::UpdateMouseButtonStates()
{
	for (int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_LAST; i++) 
	{
		int state = glfwGetMouseButton(m_Window, i);

		if (PreviousMouseButtonStates[i] == state)
			MouseButtonStates[i] = GLFW_RELEASE;

		if (state != PreviousMouseButtonStates[i])
			MouseButtonStates[i] = state;
		//else if (state != GLFW_PRESS)
			//MouseButtonStates[i] = state;

		PreviousMouseButtonStates[i] = MouseButtonStates[i];
	}
}

void InputHandler::SetWindow(GLFWwindow* window) { m_Window = window; }
GLFWwindow* InputHandler::GetWindow() { return m_Window; }

int InputHandler::MouseButtonStates[InputHandler::MouseButtonCount];
int InputHandler::PreviousMouseButtonStates[InputHandler::MouseButtonCount];
GLFWwindow* InputHandler::m_Window = nullptr;