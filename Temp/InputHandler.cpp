#include "InputHandler.h"

#include <GLFW/glfw3.h>
#include <iostream>

void InputHandler::Init(GLFWwindow* window)
{
	m_Window = window;

	// Initialize state
	for (int i = 0; i < MouseButtonCount; i++)
		MouseButtonStates[i] = GLFW_RELEASE;
}

void InputHandler::Clear()
{
	for (int i = 0; i < MouseButtonCount; i++)
		MouseButtonStates[i] = GLFW_RELEASE;
	for (int i = 0; i < 4000; i++)
		KeyStates[i] = GLFW_RELEASE;
}

bool InputHandler::IsKeyPressed(int key)
{
	return KeyStates[key] == GLFW_PRESS;
}

bool InputHandler::IsKeyHeld(int key)
{
	return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

void InputHandler::UpdateKeyState(int key, int action, int mods)
{
	KeyStates[key] = action;
}

bool InputHandler::IsMouseButtonPressed(int button)
{
	return MouseButtonStates[button] == MOUSE_BUTTON_PRESSED;
}

bool InputHandler::IsMouseButtonHeld(int button)
{
	return glfwGetMouseButton(m_Window, button) == GLFW_PRESS;
}

void InputHandler::UpdateMouseButtonState(int button, int action, int mods)
{
	MouseButtonStates[button] = action;
}
//
//void InputHandler::UpdateKeyStates()
//{
//	for (int i = 0; i < 4000; i++)
//	{
//		int state = glfwGetKey(m_Window, i);
//
//		if (PreviousKeyStates[i] == state)
//			KeyStates[i] = GLFW_RELEASE;
//
//		if (state != PreviousKeyStates[i])
//			KeyStates[i] = state;
//
//		PreviousKeyStates[i] = KeyStates[i];
//	}
//}
//
//
//void InputHandler::UpdateMouseButtonStates()
//{
//	for (int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_LAST; i++) 
//	{
//		int state = glfwGetMouseButton(m_Window, i);
//
//		if (PreviousMouseButtonStates[i] == state)
//			MouseButtonStates[i] = GLFW_RELEASE;
//
//		if (state != PreviousMouseButtonStates[i])
//			MouseButtonStates[i] = state;
//
//		PreviousMouseButtonStates[i] = MouseButtonStates[i];
//	}
//}

void InputHandler::SetWindow(GLFWwindow* window) { m_Window = window; }
GLFWwindow* InputHandler::GetWindow() { return m_Window; }

int InputHandler::MouseButtonStates[InputHandler::MouseButtonCount];
int InputHandler::PreviousMouseButtonStates[InputHandler::MouseButtonCount];
int InputHandler::KeyStates[InputHandler::KeyCount];
int InputHandler::PreviousKeyStates[InputHandler::KeyCount];

GLFWwindow* InputHandler::m_Window = nullptr;