#pragma once

#include <glm/mat4x4.hpp>

class ShadowMap
{
public:
	void Create();

	glm::mat4 BindAndGetTransformation();

	void Unbind();

public:
	unsigned int m_QuadVAO = 0; // for debug

private:
	unsigned int depthMapFBO;

	const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
};