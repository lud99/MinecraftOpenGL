#pragma once

struct GLFWwindow;

namespace PhysicsMain
{
	void Init();

	void Update(double deltaTime);

	void Render();

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
}