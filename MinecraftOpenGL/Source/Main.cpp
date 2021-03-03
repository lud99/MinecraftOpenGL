#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL // glm::vec3 hash for maps

#include <GL/glewh.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono> 
#include <vector>

#include <glm/vec3.hpp>

#include "World/World.h"
#include "InputHandler.h"
#include "World/WorldRenderer.h"
#include "World/Player/Player.h"
#include "World/Chunk/ChunkBlock.h"
#include "World/Chunk/Chunk.h"

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	World::GetPlayer().MouseCallback(window, xpos, ypos);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	World::GetPlayer().MouseButtonCallback(window, button, action, mods);
}

int main()
{
	std::cout << "sizeof ChunkBlock: " << sizeof(ChunkBlock) << "\n"; // 16 bytes
	std::cout << "sizeof u8vec3: " << sizeof(glm::u8vec3) << "\n";
	std::cout << "sizeof ivec2: " << sizeof(glm::ivec2) << "\n";
	std::cout << "sizeof uint16: " << sizeof(uint16_t) << "\n";
	std::cout << "sizeof Chunk: " << sizeof(Chunk) << "\n";

	ChunkBlock* b = new ChunkBlock();
	b->SetLocalPosition(glm::u8vec3(15, 127, 0));

	GLFWwindow* window;

	/* Initialize the library */
 	if (!glfwInit()) return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1280, 720, "MinecraftOpenGL", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "Error initializing GLEW\n";

	// Do OpenGl stuff here

	glfwSetCursorPosCallback(window, MouseCallback);
	glfwSetMouseButtonCallback(window, MouseButtonCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwWindowHint(GLFW_SAMPLES, 4);
	//glfwSwapInterval(0); // Vsync

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	// Enable opacity
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(2);

	// Print the OpenGL version
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	World::Init(window);

	InputHandler input(window);

	double previousTime = glfwGetTime();
	double prevTime = glfwGetTime();
	int frameCount = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); 

		// Close the window if escape is pressed
		if (input.IsKeyPressed(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, true);

		// Calcuate FPS
		double currentTime = glfwGetTime();
		frameCount++;

		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			std::string title = "MinecraftOpenGL | FPS: ";

			title.append(std::to_string(frameCount));

			glfwSetWindowTitle(window, title.c_str());

			frameCount = 0;
			previousTime = currentTime;
		}

		float deltaTime = glfwGetTime() - prevTime;
		prevTime = glfwGetTime();

		// Update everything in the world
		World::Update(deltaTime);

		World::Render();


		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << err << "\n";
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}