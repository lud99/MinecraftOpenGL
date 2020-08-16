#define STB_IMAGE_IMPLEMENTATION

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono> 
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include "Graphics/Shaders/ShaderLoader.h"

#include "World/Chunk/Chunk.h"
#include "World/Chunk/ChunkBlock.h"
#include "World/World.h"
#include "Graphics/Textures/TextureAtlas.h"
#include "InputHandler.h"
#include "World/WorldRenderer.h"

void CreateChunks(int count)
{
	for (int x = -count / 2; x < count / 2; x++)
	{
		for (int z = -count / 2; z < count / 2; z++)
		{
			if (World::ChunkExistsAtPosition(glm::ivec2(x, z)))
				continue;

			Chunk* chunk = World::CreateChunk(glm::ivec2(x, z));
		}
	}

	for (int x = -count / 2; x < count / 2; x++)
	{
		for (int z = -count / 2; z < count / 2; z++)
		{
			Chunk* chunk = World::GetChunkAtPosition(glm::ivec2(x, z));

			if (chunk->m_HasGenerated)
				continue;

			// Fill the chunk with blocks
			chunk->GenerateTerrain();
		}
	}

	for (int x = -count / 2; x < count / 2; x++)
	{
		for (int z = -count / 2; z < count / 2; z++)
		{
			Chunk* chunk = World::GetChunkAtPosition(glm::ivec2(x, z));

			// Create the block mesh
			chunk->RebuildMesh();
		}
	}
}

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
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit()) return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1920, 1080, "MinecraftOpenGL", NULL, NULL);
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

	glLineWidth(5);

	// Print the OpenGL version
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	World::Init(window);

	InputHandler input(window);

	auto start = std::chrono::high_resolution_clock::now();

	int count = 2;

	CreateChunks(count);

	auto stop = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Chunk generation took " << duration.count() << " seconds\n";

	double previousTime = glfwGetTime();
	int frameCount = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
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
		//	std::cout << frameCount << std::endl;

			frameCount = 0;
			previousTime = currentTime;
		}

		if (input.IsKeyPressed(GLFW_KEY_R)) {
			count += 2;

			CreateChunks(count);
		}

		// Update everything in the world
		World::Update();

		World::m_Renderer->Render();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}