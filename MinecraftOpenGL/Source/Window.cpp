#include "Window.h"

#include <GLFW/glfw3.h>

Window& Window::Get()
{
	static Window instance;
	return instance;
}