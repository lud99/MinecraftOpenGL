#pragma once

#include "Graphics/Mesh.h"
#include "Graphics/Shaders/Shader.h"

class Crosshair
{
public:
	Crosshair();

	void Render();

	inline Mesh& GetMesh() { return m_Mesh; };
	inline Shader& GetShader() { return m_Shader; }

	~Crosshair();

private:
	Mesh m_Mesh;
	Shader m_Shader;
};