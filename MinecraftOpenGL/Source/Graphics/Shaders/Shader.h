#pragma once

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>

#include <string>

class Shader
{
public:
	Shader();
	Shader(unsigned int shaderProgramId);

	void Bind();
	void Unbind();

	void SetUniform(const std::string& name, glm::mat4 matrix);
	void SetUniform(const std::string& name, float value);

	~Shader();

public:
	unsigned int m_id;
};
