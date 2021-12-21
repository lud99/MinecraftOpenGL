#define STB_IMAGE_IMPLEMENTATION
#define GLM_ENABLE_EXPERIMENTAL // glm::vec3 hash for maps

#include <GL/glewh.h>
#include <GLFW/glfw3.h>
#include <optick.h>

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

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <Common/DebugConsole.h>

#include <enet/enet.h>

#include <Common/json.hpp>

#include <Common/Net/NetworkClient.h>
#include <Common/Net/NetworkMessages.h>

#include "Network/ClientNetworkThread.h"

float Time::ElapsedTime;
float Time::DeltaTime;

bool lockedMouse = false;

void MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	ClientWorld* world = NetworkThread::Get().GetThisWorld();
	if (!world) return;
	if (!world->m_LocalPlayer) return;

	if (!lockedMouse) 
		world->m_LocalPlayer->m_FirstMouse = true;

	// Don't to mouse movements if the window isn't focused
	int focused = glfwGetWindowAttrib(window, GLFW_FOCUSED);
	if (focused && lockedMouse)
		world->m_LocalPlayer->MouseCallback(window, xpos, ypos);
}

void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureMouse)
		return;

	InputHandler::UpdateMouseButtonState(button, action, mods);

	if (!lockedMouse)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	lockedMouse = true;
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (ImGui::GetIO().WantCaptureKeyboard)
		return;

	InputHandler::UpdateKeyState(key, action, mods);
}

// Whenever the window size is changed (by OS or user resize) this has to be called to resize the gl drawing context
void WindowFramebufferResizeCallback(GLFWwindow* window, int width, int height)
{
	ClientWorld* world = NetworkThread::Get().GetThisWorld();
	if (!world) return;
	if (!world->m_LocalPlayer) return;

	world->m_LocalPlayer->GetCamera().UpdateProjectionMatrix(width, height);
	glViewport(0, 0, width, height);
}

void WindowFocusCallback(GLFWwindow* window, int focused)
{
	if (focused)
	{
		
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else
	{
		lockedMouse = false;
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

void test()
{
	std::cout << "sizeof ChunkBlock: " << sizeof(ChunkBlock) << "\n";
	std::cout << "sizeof Chunk: " << sizeof(Chunk) << "\n";
}

int main()
{
	test();

	/* Initialize the library */
	if (!glfwInit()) return -1;

	GLFWwindow* window = nullptr;
	Window::Get().m_Window = window;

	InputHandler::SetWindow(window);

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
	glfwSetFramebufferSizeCallback(window, WindowFramebufferResizeCallback);
	glfwSetWindowFocusCallback(window, WindowFocusCallback);

	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwSwapInterval(1); // Vsync

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

	// connect
	bool isConnected = NetworkThread::Get().Connect("127.0.0.1", 7777);
	if (!isConnected)
	{
		std::cout << "Could not connect to 127.0.0.1:7777\n";

	}
	else
	{
		json msg;
		msg["Type"] = "JoinWorld";
		msg["Data"]["SessionName"] = "Minecraft";
		NetworkThread::Get().m_Me->SendJson(msg);
	}

	InputHandler::Init(window);

	double previousTime = glfwGetTime();
	double prevTime = glfwGetTime();
	double prevFixedTimestempTime = glfwGetTime();
	double prevTickTime = glfwGetTime();
	int frameCount = 0;

	// Initialize imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		OPTICK_FRAME("MainThread");
		
		// Clear screen
		glClearColor(1, 1, 1, 1); // White
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Poll keys and window events
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Close the window if escape is pressed
		if (InputHandler::IsKeyPressed(GLFW_KEY_ESCAPE))
			glfwSetWindowShouldClose(window, true);

		if (InputHandler::IsKeyPressed(GLFW_KEY_F1))
		{
			Console::m_Enabled = !Console::m_Enabled;

			if (Console::m_Enabled)
			{
				lockedMouse = false;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			}
			else 
			{
				lockedMouse = true;
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			}
		}

		ClientWorld* localWorld = nullptr;
		NetworkThread& net = NetworkThread::Get();
		if (net.m_Me && net.m_Me->m_HasJoinedSession)
			localWorld = NetworkThread::Get().GetThisWorld();

		// Calcuate FPS
		double currentTime = glfwGetTime();
		frameCount++;

		if (currentTime - prevFixedTimestempTime >= Time::FixedTimestep)
		{
			if (localWorld) localWorld->CommonOnFixedUpdate();

			prevFixedTimestempTime = currentTime;
		}

		if (currentTime - prevTickTime >= Time::TickRate)
		{
			if (localWorld) localWorld->CommonOnTickUpdate();

			prevTickTime = currentTime;
		}

		// If a second has passed.
		if (currentTime - previousTime >= 1.0)
		{
			std::string title = "MinecraftOpenGL | FPS: ";
			title.append(std::to_string(frameCount));

			if (localWorld && localWorld->m_LocalPlayer)
			{
				glm::vec3 pos = localWorld->m_LocalPlayer->m_Position;

				title.append(" | " + std::to_string(pos.x) + ", " + std::to_string(pos.y) + ", " + std::to_string(pos.z));
				title.append(" | " + std::to_string(net.m_Me->m_Id));

				glfwSetWindowTitle(window, title.c_str());
			}

			frameCount = 0;
			previousTime = currentTime;
		}

		Time::ElapsedTime = glfwGetTime();
		Time::DeltaTime = Time::ElapsedTime - prevTime;

		prevTime = Time::ElapsedTime;

		if (localWorld)
		{
			localWorld->CommonOnUpdate();

			localWorld->Render();
		}

		Console::Render();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		GLenum err;
		while ((err = glGetError()) != GL_NO_ERROR)
		{
			std::cout << "OpenGL error: " << err << "\n";
		}

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		InputHandler::Clear();
	}

	OPTICK_SHUTDOWN();

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	NetworkThread::Get().m_ShouldExit = true;

	if (NetworkThread::Get().m_Thread.joinable())
		NetworkThread::Get().m_Thread.join();

	glfwTerminate();
	return 0;
}