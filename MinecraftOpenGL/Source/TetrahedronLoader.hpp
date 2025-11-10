#pragma once

// Libs
#include <vector>
#include <string>
#include <glm/vec3.hpp>

namespace TetrahedronLoader
{
	struct Edge
	{
		size_t startIndex = 0;
		size_t endIndex = 0;
		int32_t indexOfTetrahedronContainingEdge = -1;
	};

	struct Data
	{
		std::vector<glm::dvec3> nodes;
		std::vector<std::vector<size_t>> listOfTetrahedra;
		std::vector<Edge> listOfEdges;

		// Parsing state
		int32_t firstIndex = 0;
	};

	Data Parse(const std::string& filepath);
}