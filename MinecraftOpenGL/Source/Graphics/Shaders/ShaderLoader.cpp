#include "ShaderLoader.h"

#include <GL/glew.h>

#include <iostream>
#include <fstream>
#include <sstream>

std::string ShaderLoader::ParseShader(const std::string& filepath)
{
	std::ifstream stream(filepath);

	std::string line;
	std::stringstream shaderStringStream;

	while (getline(stream, line)) {
		shaderStringStream << line << "\n";
	}

	return shaderStringStream.str();
}

Shader ShaderLoader::CreateShader(const std::string& vertexPath, const std::string& fragmentPath)
{
	unsigned int program = glCreateProgram();
	unsigned int vertex = CompileShader(GL_VERTEX_SHADER, ParseShader(vertexPath));
	unsigned int fragment = CompileShader(GL_FRAGMENT_SHADER, ParseShader(fragmentPath));

	glAttachShader(program, vertex);
	glAttachShader(program, fragment);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vertex);
	glDeleteShader(fragment);

	return Shader(program);
}

unsigned int ShaderLoader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id = glCreateShader(type);
	const char* src = source.c_str();

	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Get the compilation status
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	// If the shader couldn't compile
	if (result == GL_FALSE) 
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);

		// Create char array
		char* message = (char*)alloca(length * sizeof(char));

		glGetShaderInfoLog(id, length, &length, message); // Get the actual message

		std::cout << "Failed to compile shader " << source << std::endl;
		std::cout << message << std::endl;

		glDeleteShader(id);

		return 0;
	}

	return id;
}