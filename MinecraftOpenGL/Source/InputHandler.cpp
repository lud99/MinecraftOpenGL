#include "InputHandler.h"

InputHandler::InputHandler()
{

}

bool InputHandler::IsKeyPressed(int key)
{
	return glfwGetKey(m_Window, key) == GLFW_PRESS;
}

void InputHandler::SetWindow(GLFWwindow* window) { m_Window = window; }
GLFWwindow* InputHandler::GetWindow() { return m_Window; }

InputHandler::~InputHandler()
{

}