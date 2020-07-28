#pragma once

#include <stb/stb_image.h>

#include <string>

class Texture2D
{
public:
	Texture2D();
	Texture2D(const std::string& filepath);

	void Load(const std::string& filepath);
	
	void Bind();
	void Unbind();

	~Texture2D();

public:
	unsigned int m_Texture;

	int m_Width;
	int m_Height;
	int m_NrChannels;
	unsigned char* m_Data;
};

