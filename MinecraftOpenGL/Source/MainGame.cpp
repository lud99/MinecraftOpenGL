#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL // glm::vec3 hash for maps

#include <GL/glewh.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <chrono> 
#include <vector>

#include <glm/vec3.hpp>

#include "World/ClientWorld.h"
#include "Window.h"
#include "InputHandler.h"
#include "World/WorldRenderer.h"
#include "World/Player/ClientPlayer.h"
#include <Common/Chunk/ChunkBlock.h>
#include <Common/Chunk/Chunk.h>
#include <Common/Time.h>
#include <Graphics/ModelParser.h>

#include <enet/enet.h>

#include <Common/json.hpp>

#include <Common/NetworkConnection.h>
#include <Common/NetworkMessages.h>

#include "Network/ClientNetworkThread.h"

float Time::ElapsedTime;
float Time::DeltaTime;

ClientWorld* localWorld = nullptr;

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	localWorld->m_LocalPlayer->MouseCallback(window, xpos, ypos);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	InputHandler::UpdateMouseButtonState(button, action, mods);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	InputHandler::UpdateKeyState(key, action, mods);
}

void test()
{
	std::cout << "sizeof ChunkBlock: " << sizeof(ChunkBlock) << "\n";
	std::cout << "sizeof Chunk: " << sizeof(Chunk) << "\n";
}

int main()
{
	test();

	// connect
	NetworkThread& net = net.Instance();
	bool isConnected = net.Connect("127.0.0.1", 7777);
	if (!isConnected)
		std::cout << "err!!\n";

	/*json msg;
	msg["Type"] = "JoinWorld";
	msg["Data"]["SessionName"] = "Minecraft";
	net.SendJson(msg);*/

	GLFWwindow* window = nullptr;
	Window::Get().m_Window = window;

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
	glfwSetKeyCallback(window, KeyCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwSwapInterval(0); // Vsync

	//glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);

	// Enable opacity
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glLineWidth(2);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);

	// Print the OpenGL version
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	InputHandler::Init(window);

	localWorld = new ClientWorld();

	Mesh<TextureVertex>* door = ModelParser::Parse("Resources/Models/door.obj");
	Shader sh = ShaderLoader::CreateShader("Resources/Shaders/Collider.vert", "Resources/Shaders/Collider.frag");

	double previousTime = glfwGetTime();
	double prevTime = glfwGetTime();
	double prevFixedTimestempTime = glfwGetTime();
	double prevTickTime = glfwGetTime();
	int frameCount = 0;

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClearColor(1, 1, 1, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Poll keys
		/* Poll for and process events */
		glfwPollEvents();

		// Close the window if escape is pressed
		if (InputHandler::IsKeyPressed(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, true);

		// Calcuate FPS
		double currentTime = glfwGetTime();
		frameCount++;

		if (currentTime - prevFixedTimestempTime >= Time::FixedTimestep)
		{
			localWorld->CommonOnFixedUpdate();
			//std::cout << currentTime - prevFixedTimestempTime << "\n";
			prevFixedTimestempTime = currentTime;
		}

		if (currentTime - prevTickTime >= Time::TickRate)
		{
			localWorld->CommonOnTickUpdate();
			//std::cout << currentTime - prevFixedTimestempTime << "\n";
			prevTickTime = currentTime;
		}

		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			std::string title = "MinecraftOpenGL | FPS: ";

			glm::vec3 pos = localWorld->m_LocalPlayer->m_Position;

			title.append(std::to_string(frameCount));
			title.append(" | " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));

			glfwSetWindowTitle(window, title.c_str());

			frameCount = 0;
			previousTime = currentTime;
		}

		Time::ElapsedTime = glfwGetTime();
		Time::DeltaTime = Time::ElapsedTime - prevTime;

		prevTime = Time::ElapsedTime;

		// Update everything in the world
		//net.PullPackets();

		localWorld->CommonOnUpdate();

		localWorld->Render();

		glm::mat4 mod(1.0);
		mod = glm::translate(mod, glm::vec3(0.0f, 55.0f, 0.0f));

		sh.Bind();
		sh.SetUniform("u_ProjectionMatrix", localWorld->m_Renderer->m_ProjectionMatrix);
		sh.SetUniform("u_ViewMatrix", localWorld->m_Renderer->m_ViewMatrix);
		sh.SetUniform("u_ModelMatrix", mod);
		door->Render();

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "OpenGL error: " << err << "\n";
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		InputHandler::Clear();

	}

	net.m_ShouldExit = true;

	if (net.m_Thread.joinable())
		net.m_Thread.join();

	glfwTerminate();
	return 0;
}