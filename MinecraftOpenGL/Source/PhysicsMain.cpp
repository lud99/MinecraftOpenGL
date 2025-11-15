#include "PhysicsMain.hpp"

// Libs
#include <iostream>
#include <glm/mat4x4.hpp>
#include <glm/matrix.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtx/norm.hpp>
#include <Graphics/Mesh.hpp>
#include <GLFW/glfw3.h>
#include <random>

#include "World/Player/Camera.h"
#include "InputHandler.h"
#include <Graphics/Vertex.h>
#include <Graphics/Shader.h>

#include "TetrahedronLoader.hpp"


static Mesh<PhysicsVertex> objectMesh;
static Mesh<PhysicsVertex> groundMesh;

static Shader shader;
static Camera camera;

static glm::mat4 projectionMatrix = glm::perspective(glm::radians(70.0f), 1920.0f / 1080.0f, 0.1f, 2500.0f);

struct Particle
{
	glm::dvec3 position{};
	glm::dvec3 previousPosition{};
	glm::dvec3 velocity = {};// { -0.5, 0.0, 0.3 };
	double inverseMass = 445097.0;//1.0 / 1.0;
};


static std::vector<double> initialVolumes;
static std::vector<double> initialLengths;

static std::vector<Particle> particles;

static TetrahedronLoader::Data meshData;

const double compliance = 500.0;
static double gravity = -9.82f;

static float elapsedTime = 0;

namespace PhysicsMain
{
	glm::mat4 GetViewMatrix()
	{
		return glm::lookAt(camera.m_Position, camera.m_Position + camera.m_Front, camera.m_Up);
	}

	static double RandomDouble()
	{
		unsigned seed = 42; // You can change this to any unsigned integer
		static std::mt19937 gen(seed);

		// Generate a random float between 0 and 1
		std::uniform_real_distribution<double> dist(0.0, 1.0);
		return dist(gen);
	}

	float ComputeTetrahedronVolume(const std::vector<Particle>& particles, const std::vector<size_t>& indices) {
		const glm::vec3& x1 = particles[indices[0]].position;
		const glm::vec3& x2 = particles[indices[1]].position;
		const glm::vec3& x3 = particles[indices[2]].position;
		const glm::vec3& x4 = particles[indices[3]].position;

		glm::vec3 v1 = x2 - x1;
		glm::vec3 v2 = x3 - x1;
		glm::vec3 v3 = x4 - x1;

		float volume = glm::dot(glm::cross(v1, v2), v3) / 6.0f;

		return volume;
	}

	void ComputeInitialConditions()
	{
		initialVolumes.reserve(meshData.listOfTetrahedra.size());
		initialLengths.reserve(meshData.listOfEdges.size());

		for (const auto& tetrahedraIndices : meshData.listOfTetrahedra)
		{
			initialVolumes.emplace_back(ComputeTetrahedronVolume(particles, tetrahedraIndices));
		}

		for (const auto& edge : meshData.listOfEdges)
		{
			const double length = glm::length(particles[edge.endIndex].position - particles[edge.startIndex].position);
			initialLengths.emplace_back(length);
		}
	}

	void Init()
	{
		meshData = TetrahedronLoader::Parse("Meshes/tet.1");

		std::vector<PhysicsVertex> vertices; /*= {
			{  glm::vec3(1,  1,  1), glm::vec4(1.0, 0.5, 0.0, 1.0), {}},
			{ glm::vec3(-1,  -1,  1), glm::vec4(1.0, 0.0, 1.0, 1.0), {}},
			{ glm::vec3(-1,  1,  -1), glm::vec4(0.0, 1.0, 1.0, 1.0), {}},
			{ glm::vec3(1,  -1,  -1), glm::vec4(0.0, 0.0, 1.0, 1.0), {}}
		};*/

		vertices.reserve(meshData.nodes.size());
		particles.reserve(meshData.nodes.size());

		const auto meshScale = 3.0;
		for (size_t i = 0; i < meshData.nodes.size(); i++)
		{
			vertices.emplace_back(meshData.nodes[i] * meshScale, 
				glm::dvec4(RandomDouble(), RandomDouble(), RandomDouble(), 1.0));



			particles.emplace_back(meshData.nodes[i] * meshScale).velocity = 
				(glm::dvec3(RandomDouble(), RandomDouble(), RandomDouble()) - 0.5) * 5.0;
		}

		ComputeInitialConditions();

		//particles[0].position = glm::vec3(1, 1, 1);// *10.0f;
		//particles[0].velocity = glm::vec3(1, 10, 1);
		//particles[1].position = glm::vec3(-1, -1, 1);
		//particles[2].position = glm::vec3(-1, 1, -1);
		//particles[2].velocity =  glm::vec3(-1, 1, -1);
		//particles[3].position = glm::vec3(1, -1, -1);

		std::vector<uint16_t> indices;
		indices.reserve(meshData.listOfFaces.size());
		for (const auto& face : meshData.listOfFaces)
		{
			for (const auto index : face.nodesIndices)
			{
				indices.emplace_back(static_cast<uint16_t>(index));
			}
		}

		objectMesh.SetIndices(indices);
		//// Create mesh
	 //   objectMesh.SetIndices({
	 //       0, 1, 2,   // Face 1
	 //       0, 3, 1,   // Face 2
	 //       0, 2, 3,   // Face 3
	 //       1, 3, 2    // Face 4
	 //   });
		objectMesh.UpdateVertices(vertices);

		std::vector<PhysicsVertex> groundVertices = {
			{  glm::vec3(-10,  -5, -10), glm::vec4(0.0, 0.0, 0.0, 1.0), {}},
			{ glm::vec3(-10,  -5, 10), glm::vec4(0.0, 0.0, 0.0, 1.0), {}},
			{ glm::vec3(10,  -5, 10), glm::vec4(0.0, 0.0, 0.0, 1.0), {}},
			{ glm::vec3(10,  -5, -10), glm::vec4(0.0, 0.0, 0.0, 1.0), {}}
		};
		groundMesh.SetIndices({ 0, 1, 2, 2, 3, 0 });
		groundMesh.UpdateVertices(groundVertices);


		shader = ShaderLoader::CreateShader("Resources/Shaders/basic.vs", "Resources/Shaders/basic.fs");

		camera.m_Position = glm::vec3(-10, 0, 0);
		camera.m_Front = glm::vec3(1.0, 0.0, 0.0);
		camera.m_Up = glm::vec3(0.0, 1.0, 0.0);
	}

	void HandleInput()
	{
		glm::vec3 newMovementDirection{};
		if (InputHandler::IsKeyHeld(GLFW_KEY_W))
			newMovementDirection += glm::normalize(camera.m_Front2D);
		if (InputHandler::IsKeyHeld(GLFW_KEY_A))
			newMovementDirection -= glm::normalize(glm::cross(camera.m_Front2D, camera.m_Up));
		if (InputHandler::IsKeyHeld(GLFW_KEY_S))
			newMovementDirection -= glm::normalize(camera.m_Front2D);
		if (InputHandler::IsKeyHeld(GLFW_KEY_D))
			newMovementDirection += glm::normalize(glm::cross(camera.m_Front2D, camera.m_Up));

		if (InputHandler::IsKeyHeld(GLFW_KEY_T))
		{
			particles[0].previousPosition = particles[0].position;
			particles[0].position = glm::dvec3(0.0, 5.0, 0.0);

			//particles[0].velocity = glm::dvec3(1.0);
		}

		camera.m_Position += newMovementDirection * 0.05f;

		if (InputHandler::IsKeyHeld(GLFW_KEY_SPACE))
			camera.m_Position.y += 0.05f; //0.212132034356f;//6.f * Time::FixedTimestep * m_Acceleration;
		if (InputHandler::IsKeyHeld(GLFW_KEY_LEFT_SHIFT))
			camera.m_Position.y -= 0.05f;// -6.f;// *Time::FixedTimestep* m_Acceleration;

		// Is undefined if x and z are equal to 0
		if (newMovementDirection.x != 0 || newMovementDirection.z != 0)
			newMovementDirection = glm::normalize(newMovementDirection);
	}

	void Render()
	{
		shader.Bind();

		shader.SetUniform("u_ProjectionMatrix", projectionMatrix);
		shader.SetUniform("u_ViewMatrix", GetViewMatrix());
		shader.SetUniform("u_Time", elapsedTime);

		groundMesh.Render();
		objectMesh.Render();

	}

	

	void SolveDistanceConstraint(double deltaTime)
	{
		// Iterate all edges
		for (const auto& edge : meshData.listOfEdges)
		{
			const double edgeRestLength = initialLengths[edge.index];
			const double alpha = compliance / (deltaTime * deltaTime);

			auto w0 = particles[edge.startIndex].inverseMass;
			auto w1 = particles[edge.endIndex].inverseMass;
			auto w = w0 + w1;
			if (w == 0.0)
			{
				continue;
			}

			const auto length = glm::length(particles[edge.startIndex].position - particles[edge.endIndex].position);
			if (length == 0.0)
			{
				continue;
			}

			glm::dvec3 gradient = glm::normalize(particles[edge.startIndex].position - particles[edge.endIndex].position);

			auto C = length - edgeRestLength;
			const auto s = -C / (w + alpha); // lambda

			// Compute and apply correction vector
			particles[edge.startIndex].position += gradient * s * w0;
			particles[edge.endIndex].position += -gradient * s * w1;
		}
	}

	void SolveVolumeConstraint(double deltaTime)
	{
		size_t tetrahedronIndex = 0;
		for (const auto& tetrahedraIndices : meshData.listOfTetrahedra)
		{
			double restVolume = initialVolumes[tetrahedronIndex];

			const double alpha = compliance / (deltaTime * deltaTime);

			// The denominator in the lambda = .. expression
			double wWeightedSum = 0.0;

			const glm::dvec3& x1 = particles[tetrahedraIndices[0]].position;
			const glm::dvec3& x2 = particles[tetrahedraIndices[1]].position;
			const glm::dvec3& x3 = particles[tetrahedraIndices[2]].position;
			const glm::dvec3& x4 = particles[tetrahedraIndices[3]].position;

			glm::dvec3 gradients[4];

			// Why divide by 6 ..?? idk
			gradients[0] = glm::cross(x4 - x2, x3 - x2) / 6.0;   // ∇₁C
			gradients[1] = glm::cross(x3 - x1, x4 - x1) / 6.0;   // ∇₂C
			gradients[2] = glm::cross(x4 - x1, x2 - x1) / 6.0;   // ∇₃C
			gradients[3] = glm::cross(x2 - x1, x3 - x1) / 6.0;   // ∇₄C

			for (int i = 0; i < 4; i++)
			{
				wWeightedSum += particles[tetrahedraIndices[i]].inverseMass * glm::length2(gradients[i]);
			}

			assert(wWeightedSum != 0.0);

			const auto volume = ComputeTetrahedronVolume(particles, tetrahedraIndices);
			auto C = volume - restVolume;
			auto s = -C / (wWeightedSum + alpha);

			for (int i = 0; i < 4; i++) {
				particles[tetrahedraIndices[i]].position += gradients[i] * s * particles[tetrahedraIndices[i]].inverseMass;
			}

			++tetrahedronIndex;
		}

	}

	void Solve(double deltaTime)
	{
		SolveDistanceConstraint(deltaTime);
		SolveVolumeConstraint(deltaTime);
	}

	void Update(double deltaTime)
	{
		HandleInput();

		elapsedTime += deltaTime;

		// Solve
		const int constraintIterations = 20;
		double substeps = 10.0;
		double deltaTimeS = deltaTime / substeps;
		for (int n = 0; n < substeps; n++)
		{
			for (int i = 0; i < particles.size(); i++)
			{
				particles[i].velocity += deltaTimeS * glm::dvec3(0.0, gravity, 0.0);
				particles[i].previousPosition = particles[i].position;
				particles[i].position += deltaTimeS * particles[i].velocity;

				// Ground collision
				if (particles[i].position.y < -5.0)
				{
					particles[i].position = particles[i].previousPosition;
					particles[i].velocity = {};
				}
			}

			// Solve all constraints
			for (int iter = 0; iter < constraintIterations; iter++)
			{
				Solve(deltaTimeS);

			}
			// Post solve
			for (int i = 0; i < particles.size(); i++) {
				if (particles[i].inverseMass == 0.0)
				{
					continue;
				}

				particles[i].velocity = (particles[i].position - particles[i].previousPosition) / deltaTimeS;

				//vecSetDiff(this.vel, i, this.pos, i, this.prevPos, i, 1.0 / dt);
			}

			
			//std::cout << particles[0].velocity.y << "\n";
		}

		
		// Update vertices
		for (int i = 0; i < particles.size(); i++)
		{
			objectMesh.GetVertices()[i].position = particles[i].position;
		}
		objectMesh.Update();


		Render();
	}


	void MouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		float x = (float)xpos;
		float y = (float)ypos;

		static bool firstMouse = true;
		static float lastMouseX, lastMouseY;

		static double mouseYaw, mousePitch;

		bool mouseSensitivity = 0.001;

		if (firstMouse)
		{
			lastMouseX = x;
			lastMouseY = y;
			firstMouse = false;
		}

		float xoffset = (x - lastMouseX) * mouseSensitivity;
		float yoffset = (lastMouseY - y) * mouseSensitivity;

		mouseYaw += xoffset;
		mousePitch += yoffset;

		if (mousePitch > 89.0f)
			mousePitch = 89.0f;
		if (mousePitch < -89.0f)
			mousePitch = -89.0f;

		glm::vec3 direction2D(0.0f);
		direction2D.x = cos(glm::radians(mouseYaw));
		direction2D.y = 0;
		direction2D.z = sin(glm::radians(mouseYaw));
		camera.m_Front2D = glm::normalize(direction2D);

		glm::vec3 direction(0.0f);
		direction.x = direction2D.x * cos(glm::radians(mousePitch));
		direction.y = sin(glm::radians(mousePitch));
		direction.z = direction2D.z * cos(glm::radians(mousePitch));
		camera.m_Front = glm::normalize(direction);

		lastMouseX = x;
		lastMouseY = y;
	}

}