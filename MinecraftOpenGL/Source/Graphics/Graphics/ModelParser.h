#pragma once

#include <string>
#include <vector>

template <typename T> class Mesh;
struct TextureVertex;

class ModelParser
{
public:
	static void Parse(const std::string& filepath, Mesh<TextureVertex>& model);

private:

};