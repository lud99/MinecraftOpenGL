#include "PhysicsMain.hpp"

#include <Graphics/Mesh.hpp>
#include <Graphics/Vertex.h>

static Mesh<PhysicsVertex> objectMesh;
static Mesh<PhysicsVertex> groundMesh;

void Init()
{
    glm::vec3 tetrahedronVertices[4] = {
    glm::vec3(1,  1,  1),
    glm::vec3(-1, -1,  1),
    glm::vec3(-1,  1, -1),
    glm::vec3(1, -1, -1)
    };

    std::vector<PhysicsVertex>

	// Create mesh
	objectMesh.UpdateVerticies()
}