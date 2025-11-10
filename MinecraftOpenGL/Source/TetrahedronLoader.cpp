// edges .edge
// faces .face
// points .node
// list of tets (.ele)
// todo neigh


#include <vector>
#include <string>
#include <fstream>
#include <iostream>
#include <glm/vec3.hpp>
#include <sstream>

#include "TetrahedronLoader.hpp"

namespace TetrahedronLoader
{
	// TODO:
	// TetGen default only outputs the boundary faces or the convex hull faces into a .face file. If the -f switch is used, 
	// // TetGen outputs all faces (including interior faces) of the tetrahedralization. 
	// In this case, each interior face will always have a ‘0’ as its boundary marker. This file can be omitted by using the -F switch.
	// ^ research that 

	// use -nn to get all edges

	// Should probably get external faces only aswell 

	static void ParseEdges(const std::string& filepath, Data& data)
	{
		auto file = std::ifstream(filepath + ".edge");
		if (!file.good())
		{
			std::cout << "Error loading " << filepath << "\n";
			return;
		}

		std::string line;

		// Read header
		std::getline(file, line);
		std::istringstream headerStream(line);
		double tetrahedra, boundaryMarker;
		headerStream >> tetrahedra >> boundaryMarker;

		// Assert only 2 points per edge (no endpoint)
		assert(boundaryMarker == 1);

		std::cout << "Header: " << tetrahedra << " " << boundaryMarker << "\n";

		struct Row {
			std::string index;
			size_t startNode;
			size_t endNode;
			int32_t boundaryMarker;
			int32_t indexOfTetrahedronContainingEdge;
		};

		// Read remaining lines into a vector
		Row temp{};
		while (file >> std::ws and std::getline(file >> std::ws, line)) {
			std::stringstream iss(line);
			if (iss >> temp.index >> temp.startNode >> temp.endNode >> temp.boundaryMarker >> temp.indexOfTetrahedronContainingEdge) {
				data.listOfEdges.emplace_back(temp.startNode - data.firstIndex, temp.endNode - data.firstIndex, temp.indexOfTetrahedronContainingEdge - data.firstIndex);

			}
		}



		// Print the parsed data
		std::cout << "Parsed list of edges\n";
		for (const auto& row : data.listOfEdges) {
			std::cout
				<< row.startIndex << " "
				<< row.endIndex << " "
				<< row.indexOfTetrahedronContainingEdge << "\n";
			//<< row.value << "\n";
		}
	}

	static void ParseTetrahedraList(const std::string& filepath, Data& data)
	{
		auto file = std::ifstream(filepath + ".ele");
		if (!file.good())
		{
			std::cout << "Error loading " << filepath << "\n";
			return;
		}

		std::string line;

		// Read header
		std::getline(file, line);
		std::istringstream headerStream(line);
		double tetrahedraCount, nodesPerTetrahedra, regionAttribute;
		headerStream >> tetrahedraCount >> nodesPerTetrahedra >> regionAttribute;

		assert(nodesPerTetrahedra == 4);

		std::cout << "Header: " << tetrahedraCount << " " << nodesPerTetrahedra << " " << regionAttribute << "\n";

		struct Row {
			std::string index;
			std::vector<size_t> nodes;
		};

		// Read remaining lines into a vector
		Row temp{};
		while (file >> std::ws and std::getline(file >> std::ws, line)) {
			temp.nodes.resize(nodesPerTetrahedra);

			std::stringstream iss(line);
			if (iss >> temp.index >> temp.nodes[0] >> temp.nodes[1] >> temp.nodes[2] >> temp.nodes[3]) {
				// Make zero-indexed
				for (size_t i = 0; i < 4; i++)
				{
					temp.nodes[i] -= data.firstIndex;
				}
				data.listOfTetrahedra.push_back(temp.nodes);
			}
		}

		// Print the parsed data
		std::cout << "Parsed list of tetrahedra\n";
		for (const auto& row : data.listOfTetrahedra) {
			std::cout
				<< row[0] << " "
				<< row[1] << " "
				<< row[2] << " "
				<< row[3] << "\n";
		}
	}

	Data Parse(const std::string& filepath)
	{
		Data data{};

		auto file = std::ifstream(filepath + ".node");
		if (!file.good())
		{
			std::cout << "Error loading " << filepath << "\n";
			return {};
		}

		std::string line;

		// Read header
		std::getline(file, line);
		std::istringstream headerStream(line);
		int32_t nodeCount, dimensions, hasAttribute, hasBoundaryMarker;
		headerStream >> nodeCount >> dimensions >> hasAttribute >> hasBoundaryMarker;

		assert(dimensions == 3);
		assert(hasAttribute == 0);

		std::cout << "Header: " << nodeCount << " " << dimensions << " " << hasAttribute << " " << hasBoundaryMarker << "\n";

		struct Row {
			size_t index;
			double x;
			double y;
			double z;
			int boundaryMarker;
		};

		bool firstIteration = true;

		// Read remaining lines into a vector
		Row temp{};
		while (file >> std::ws and std::getline(file >> std::ws, line)) {

			std::cout << line << "\n";

			std::stringstream iss(line);
			if (iss >> temp.index >> temp.x >> temp.y >> temp.z)
			{
				// Some files have 0 index and other 1 index for some reason
				if (firstIteration)
				{
					data.firstIndex = temp.index;
					firstIteration = false;
				}

				data.nodes.push_back(glm::dvec3(temp.x, temp.y, temp.z));
			}
			if (hasBoundaryMarker)
			{
				iss >> temp.boundaryMarker;
			}
		}



		// Print the parsed data
		std::cout << "Parsed nodes\n";
		std::cout << "start index: " << data.firstIndex << "\n";
		for (const auto& row : data.nodes) {
			std::cout
				<< row.x << " "
				<< row.y << " "
				<< row.z << "\n";
			//<< row.value << "\n";
		}

		ParseTetrahedraList(filepath, data);

		ParseEdges(filepath, data);

		return data;
	}
}