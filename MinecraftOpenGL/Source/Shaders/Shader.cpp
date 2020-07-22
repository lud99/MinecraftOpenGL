#include "Shader.h"

#include <GL/glew.h>

#include <glm/gtc/type_ptr.hpp>

Shader::Shader(unsigned int shaderProgramId) : m_id(shaderProgramId)
{
}

void Shader::Bind()
{
	glUseProgram(m_id);
}

void Shader::SetUniform(const std::string& name, glm::mat4 matrix)
{
	int location = glGetUniformLocation(m_id, name.c_str());

	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}


Shader::~Shader()
{
}
