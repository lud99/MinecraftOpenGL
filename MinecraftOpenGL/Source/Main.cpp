#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono> 
#include <vector>

#include <glm/vec3.hpp>
#include <glm/gtc/matrix_transform.hpp> 

#include "Shaders/ShaderLoader.h"

#include "Chunk/Chunk.h"
#include "Chunk/ChunkBlock.h"
#include "World.h"

float lastX = 400, lastY = 300;
float yaw = 0, pitch = 0;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse) // initially set to true
	{
		lastX = (float)xpos;
		lastY = (float)ypos;
		firstMouse = false;
	}

	float xoffset = (float)xpos - lastX;
	float yoffset = lastY - (float)ypos;
	lastX = (float)xpos;
	lastY = (float)ypos;

	float sensitivity = 0.1f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 direction;
	direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	direction.y = sin(glm::radians(pitch));
	direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(direction);
}

int main()
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(1920, 1080, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK)
		std::cout << "Error initializing GLEW\n";

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwSetCursorPosCallback(window, mouse_callback);

	// Print the OpenGL version
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	const glm::vec4 colors[36] = {
		glm::vec4(0.583f,  0.771f,  0.014f, 1.0f),
		glm::vec4(0.609f,  0.115f,  0.436f, 1.0f),
		glm::vec4(0.327f,  0.483f,  0.844f, 1.0f),
		glm::vec4(0.822f,  0.569f,  0.201f, 1.0f),
		glm::vec4(0.435f,  0.602f,  0.223f, 1.0f),
		glm::vec4(0.310f,  0.747f,  0.185f, 1.0f),
		glm::vec4(0.597f,  0.770f,  0.761f, 1.0f),
		glm::vec4(0.559f,  0.436f,  0.730f, 1.0f),
		glm::vec4(0.359f,  0.583f,  0.152f, 1.0f),
		glm::vec4(0.483f,  0.596f,  0.789f, 1.0f),
		glm::vec4(0.559f,  0.861f,  0.639f, 1.0f),
		glm::vec4(0.195f,  0.548f,  0.859f, 1.0f),
		glm::vec4(0.014f,  0.184f,  0.576f, 1.0f),
		glm::vec4(0.771f,  0.328f,  0.970f, 1.0f),
		glm::vec4(0.406f,  0.615f,  0.116f, 1.0f),
		glm::vec4(0.676f,  0.977f,  0.133f, 1.0f),
		glm::vec4(0.971f,  0.572f,  0.833f, 1.0f),
		glm::vec4(0.140f,  0.616f,  0.489f, 1.0f),
		glm::vec4(0.997f,  0.513f,  0.064f, 1.0f),
		glm::vec4(0.945f,  0.719f,  0.592f, 1.0f),
		glm::vec4(0.543f,  0.021f,  0.978f, 1.0f),
		glm::vec4(0.279f,  0.317f,  0.505f, 1.0f),
		glm::vec4(0.167f,  0.620f,  0.077f, 1.0f),
		glm::vec4(0.347f,  0.857f,  0.137f, 1.0f),
		glm::vec4(0.055f,  0.953f,  0.042f, 1.0f),
		glm::vec4(0.714f,  0.505f,  0.345f, 1.0f),
		glm::vec4(0.783f,  0.290f,  0.734f, 1.0f),
		glm::vec4(0.722f,  0.645f,  0.174f, 1.0f),
		glm::vec4(0.302f,  0.455f,  0.848f, 1.0f),
		glm::vec4(0.225f,  0.587f,  0.040f, 1.0f),
		glm::vec4(0.517f,  0.713f,  0.338f, 1.0f),
		glm::vec4(0.053f,  0.959f,  0.120f, 1.0f),
		glm::vec4(0.393f,  0.621f,  0.362f, 1.0f),
		glm::vec4(0.673f,  0.211f,  0.457f, 1.0f),
		glm::vec4(0.820f,  0.883f,  0.371f, 1.0f),
		glm::vec4(0.982f,  0.099f,  0.879f, 1.0f),
	};

	const int chunkCount = 24;

	// 1160mb
	auto start = std::chrono::high_resolution_clock::now();

	for (int x = 0; x < chunkCount / 2; x++)
	{
		for (int z = 0; z < chunkCount / 2; z++)
		{
			Chunk* chunk = World::CreateChunk(glm::vec3(Chunk::Width * x, 0, Chunk::Depth * z));

			std::cout << "Chunk position: " << Chunk::Width * x << ", " << 0 << ", " << Chunk::Depth * z << "\n";

			// Fill the chunk with blocks
			chunk->Fill(colors);

			// Create the block mesh
			chunk->UpdateMesh();

			std::cout << chunk->GetIndex() << "\n";
		}
	}

	auto stop = std::chrono::high_resolution_clock::now();

	auto duration = std::chrono::duration_cast<std::chrono::seconds>(stop - start);
	std::cout << "Seconds: " << duration.count() << std::endl;

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glfwSwapInterval(0);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load shaders
	Shader shader = ShaderLoader::CreateShader("Resources/Shaders/Basic.vert", "Resources/Shaders/Basic.frag");

	shader.Bind();

	glm::mat4 projection = glm::perspective(glm::radians(70.0f), 1920.0f / 1080.0f, 0.1f, 1000.0f);

	float cameraSpeed = 0.05f; // adjust accordingly

	double previousTime = glfwGetTime();
	int frameCount = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Calcuate FPS
		double currentTime = glfwGetTime();
		frameCount++;

		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			std::cout << frameCount << std::endl;

			frameCount = 0;
			previousTime = currentTime;
		}

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			cameraPos.y += cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			cameraPos.y -= cameraSpeed;

		if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
			cameraSpeed -= 0.025f;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			cameraSpeed += 0.025f;

		// Remove focus from the window
		if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		// Exit
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);
		
		glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		shader.SetUniform("u_ProjectionMatrix", projection);
		shader.SetUniform("u_ViewMatrix", view);

		World::Render();

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}