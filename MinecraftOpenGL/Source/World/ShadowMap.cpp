#include "ShadowMap.h"

#include <GL/glewh.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <iostream>

void ShadowMap::Create()
{
	glGenFramebuffers(1, &depthMapFBO);

	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE); // Creates without color buffer
	glReadBuffer(GL_NONE); // same here

	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer not complete! Error: " << status << std::endl;
	}


	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// for debug
	glGenVertexArrays(1, &m_QuadVAO);
}

glm::mat4 ShadowMap::BindAndGetTransformation()
{
	// 1. first render to depth map
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

	float near_plane = 1.0f, far_plane = 20.5f;
	glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);

	auto lightPos = glm::vec3(-40.0f, 0.0f, 60.0f);
	glm::mat4 lightView = glm::lookAt(lightPos,
		glm::vec3(0.0f, 0.0f, 0.0f), // world origin
		glm::vec3(0.0f, 1.0f, 0.0f)); // up
	
	glBindTexture(GL_TEXTURE_2D, depthMap);


	return lightProjection * lightView;

	//ConfigureShaderAndMatrices();
	//RenderScene();
	//glBindFramebuffer(GL_FRAMEBUFFER, 0);
	//// 2. then render scene as normal with shadow mapping (using depth map)
	//glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//ConfigureShaderAndMatrices();
	//glBindTexture(GL_TEXTURE_2D, depthMap);
	//RenderScene();
}

void ShadowMap::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
