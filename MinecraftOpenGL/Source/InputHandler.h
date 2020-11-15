#pragma once

struct GLFWwindow;

class InputHandler
{
public:
	InputHandler();
	InputHandler(GLFWwindow* window) : m_Window(window) {};

	bool IsKeyPressed(int key);

	void SetWindow(GLFWwindow* window);
	GLFWwindow* GetWindow();

	~InputHandler();
public:
	GLFWwindow* m_Window = nullptr;
};