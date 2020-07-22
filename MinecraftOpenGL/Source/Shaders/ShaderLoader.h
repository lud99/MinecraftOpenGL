#pragma once

#include <string>

#include "Shader.h"

namespace ShaderLoader
{
	std::string ParseShader(const std::string& filepath);
	unsigned int CompileShader(unsigned int type, const std::string& source);

	Shader CreateShader(const std::string& vertexPath, const std::string& fragmentPath);
};

