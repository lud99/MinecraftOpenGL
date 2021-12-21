#pragma once

#include <string>
#include <vector>

template <typename T> class Mesh;
struct TextureVertex;

class ModelParser
{
public:
	static void Parse(const std::string& dir, const std::string& filename, Mesh<TextureVertex>& model);

private:

};