#pragma once

struct GLFWwindow;

class Window
{
public:
	static Window& Get();

private:
	Window() {};

public:
	GLFWwindow* m_Window = nullptr;

private:
};