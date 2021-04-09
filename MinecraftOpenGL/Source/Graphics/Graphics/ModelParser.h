#pragma once

#include <string>

template <typename T> class Mesh;
struct TextureVertex;

class ModelParser
{
public:
	static Mesh<TextureVertex>* Parse(const std::string& filepath);

private:

};