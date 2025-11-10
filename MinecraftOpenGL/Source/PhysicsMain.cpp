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

static Particle particles[4];

const double compliance = 500.0;
static double gravity = -9.82f;

namespace PhysicsMain
{
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(camera.m_Position, camera.m_Position + camera.m_Front, camera.m_Up);
    }

    void Init()
    {
        TetrahedronLoader::Parse("Meshes/example.1");

        std::vector<PhysicsVertex> vertices = {
            {  glm::vec3(1,  1,  1), glm::vec4(1.0, 0.5, 0.0, 1.0), {}},
            { glm::vec3(-1,  -1,  1), glm::vec4(1.0, 0.0, 1.0, 1.0), {}},
            { glm::vec3(-1,  1,  -1), glm::vec4(0.0, 1.0, 1.0, 1.0), {}},
            { glm::vec3(1,  -1,  -1), glm::vec4(0.0, 0.0, 1.0, 1.0), {}}
        };
        particles[0].position = glm::vec3(1, 1, 1);// *10.0f;
        particles[0].velocity = glm::vec3(1, 10, 1);
        particles[1].position = glm::vec3(-1, -1, 1);
        particles[2].position = glm::vec3(-1, 1, -1);
        particles[2].velocity =  glm::vec3(-1, 1, -1);
        particles[3].position = glm::vec3(1, -1, -1);

	    // Create mesh
        objectMesh.SetIndices({
            0, 1, 2,   // Face 1
            0, 3, 1,   // Face 2
            0, 2, 3,   // Face 3
            1, 3, 2    // Face 4
        });
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

        camera.m_Position += newMovementDirection * 0.1f;

        if (InputHandler::IsKeyHeld(GLFW_KEY_SPACE))
            camera.m_Position.y += 0.1f; //0.212132034356f;//6.f * Time::FixedTimestep * m_Acceleration;
        if (InputHandler::IsKeyHeld(GLFW_KEY_LEFT_SHIFT))
            camera.m_Position.y -= 0.1f;// -6.f;// *Time::FixedTimestep* m_Acceleration;

        // Is undefined if x and z are equal to 0
        if (newMovementDirection.x != 0 || newMovementDirection.z != 0)
            newMovementDirection = glm::normalize(newMovementDirection);
    }

    void Render()
    {
        shader.Bind();

        shader.SetUniform("u_ProjectionMatrix", projectionMatrix);
        shader.SetUniform("u_ViewMatrix", GetViewMatrix());

        groundMesh.Render();
        objectMesh.Render();
    }

    float ComputeTetrahedronVolume(Particle* particles, int i1, int i2, int i3, int i4) {
        const glm::vec3& x1 = particles[i1].position;
        const glm::vec3& x2 = particles[i2].position;
        const glm::vec3& x3 = particles[i3].position;
        const glm::vec3& x4 = particles[i4].position;

        glm::vec3 v1 = x2 - x1;
        glm::vec3 v2 = x3 - x1;
        glm::vec3 v3 = x4 - x1;

        float volume = glm::dot(glm::cross(v1, v2), v3) / 6.0f;

        return volume;
    }

    void SolveDistanceConstraint(double deltaTime)
    {
        std::unordered_map<uint32_t, std::tuple<uint32_t, uint32_t>> edgeToVertex =
        {
            {0, std::make_tuple(0, 1)},
            {1, std::make_tuple(0, 2)},
            {2, std::make_tuple(0, 3)},

            {3, std::make_tuple(1, 2)},
            {4, std::make_tuple(1, 3)},
            {5, std::make_tuple(2, 3)},
        };


        const double edgeRestLength = std::sqrt(8);
        const double alpha = compliance / (deltaTime * deltaTime);

        // Iterate all edges
        for (int i = 0; i < 6; i++)
        {
            auto [id0, id1] = edgeToVertex[i];

            auto w0 = particles[id0].inverseMass;
            auto w1 = particles[id1].inverseMass;
            auto w = w0 + w1;
            if (w == 0.0)
            {
                continue;
            }

            const auto length = glm::length(particles[id0].position - particles[id1].position);
            if (length == 0.0)
            {
                continue;
            }

            glm::dvec3 gradient = normalize(particles[id0].position - particles[id1].position);

            auto C = length - edgeRestLength;
            const auto s = -C / (w + alpha); // lambda

            // Compute and apply correction vector
            particles[id0].position += gradient * s * w0;
            particles[id1].position += -gradient * s * w1;
        }
    }

    void SolveVolumeConstraint(double deltaTime)
    {
        double restVolume = 8.0 / 3.0; // From ChatGPT..

        const double alpha = compliance / (deltaTime * deltaTime);

        // The denominator in the lambda = .. expression
        double wWeightedSum = 0.0;

        const glm::dvec3& x1 = particles[0].position;
        const glm::dvec3& x2 = particles[1].position;
        const glm::dvec3& x3 = particles[2].position;
        const glm::dvec3& x4 = particles[3].position;

        glm::dvec3 gradients[4];

        // Why divide by 6 ..?? idk
        gradients[0] = glm::cross(x4 - x2, x3 - x2) / 6.0;   // ∇₁C
        gradients[1] = glm::cross(x3 - x1, x4 - x1) / 6.0;   // ∇₂C
        gradients[2] = glm::cross(x4 - x1, x2 - x1) / 6.0;   // ∇₃C
        gradients[3] = glm::cross(x2 - x1, x3 - x1) / 6.0;   // ∇₄C

        for (int i = 0; i < 4; i++)
        {
            wWeightedSum += particles[i].inverseMass * glm::length2(gradients[i]);
        }

        if (wWeightedSum == 0.0)
        {
            abort();
            return;
        }

        const auto volume = ComputeTetrahedronVolume(particles, 0, 1, 2, 3);
        auto C = volume - restVolume;
        auto s = -C / (wWeightedSum + alpha);

        for (int i = 0; i < 4; i++) {
            particles[i].position += gradients[i] * s * particles[i].inverseMass;
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

        // Solve
        double substeps = 20.0;
        double deltaTimeS = deltaTime / substeps;
        for (int n = 0; n < substeps; n++)
        {
            for (int i = 0; i < 4; i++)
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
            Solve(deltaTimeS);

            // Post solve
            for (int i = 0; i < 4; i++) {
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
        for (int i = 0; i < 4; i++)
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