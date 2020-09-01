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
	int location = glGetUniformLocation(m_id, name.c_str());

	glUniform2f(location, vector.x, vector.y);
}

void Shader::SetUniform(const std::string& name, glm::ivec2 vector)
{
	int location = glGetUniformLocation(m_id, name.c_str());

	glUniform2i(location, vector.x, vector.y);
}

void Shader::SetUniform(const std::string& name, glm::vec3 vector)
{
	int location = glGetUniformLocation(m_id, name.c_str());

	glUniform3f(location, vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, glm::ivec3 vector)
{
	int location = glGetUniformLocation(m_id, name.c_str());

	glUniform3i(location, vector.x, vector.y, vector.z);
}

void Shader::SetUniform(const std::string& name, glm::mat4 matrix)
{
	int location = glGetUniformLocation(m_id, name.c_str());

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}

void Shader::SetUniform(const std::string& name, float value)
{
	int location = glGetUniformLocation(m_id, name.c_str());

	glUniform1f(location, value);
}

Shader::~Shader()
{
}
