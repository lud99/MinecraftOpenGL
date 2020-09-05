#include "Shader.h"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>
#include <iostream>

Shader::Shader()
{

}

Shader::Shader(unsigned int shaderProgramId) : m_id(shaderProgramId)
{

}

int Shader::GetUniformLocation(const std::string& name)
{
	// Return it if it's already cached
	if (m_UniformLocations.count(name) > 0)
		return m_UniformLocations[name];

	int location = glGetUniformLocation(m_id, name.c_str());

	m_UniformLocations[name] = location;

	return location;
}

void Shader::Bind()
{
	glUseProgram(m_id);
}

void Shader::Unbind()
{
	glUseProgram(0);
}

void Shader::SetUniform(const std::string& name, glm::vec2 vector)
{
	glUniform2f(GetUniformLocation(name.c_str()), vector.x, vector.y);
}

void Shader::SetUniform(const std::string& name, glm::ivec2 vector)
{
	glUniform2i(GetUniformLocation(name.c_str()), vector.x, vector.y);
}

void Shader::SetUniform(const std::string& name, glm::vec3 vector)
{
	glUniform3f(GetUniformLocation(name.c_str()), vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, glm::ivec3 vector)
{
	glUniform3i(GetUniformLocation(name.c_str()), vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, glm::mat4 matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name.c_str()), 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform(const std::string& name, float value)
{
	glUniform1f(GetUniformLocation(name.c_str()), value);
}

Shader::~Shader()
{
}
