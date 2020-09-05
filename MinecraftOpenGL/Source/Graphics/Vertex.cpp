#include "Vertex.h"

void Vertex::SetData(VertexData vertexData)
{
	// 0 - 255 = 8 bits
	// 0 - 15 = 4 bits

	packedData = (packedData << 5) | vertexData.position.x;
	packedData = (packedData << 9) | vertexData.position.y;
	packedData = (packedData << 5) | vertexData.position.z;

	packedData = (packedData << 2) | vertexData.index;
	packedData = (packedData << 6) | vertexData.texture;
	packedData = (packedData << 4) | vertexData.lightLevel;
}

VertexData Vertex::GetData()
{
	VertexData data;

	data.position.x = (packedData >> 26)&((1 << 5) - 1);
	data.position.y = (packedData >> 17)&((1 << 9) - 1);
	data.position.z = (packedData >> 12)&((1 << 5) - 1);

	data.index = (packedData >> 10)&((1 << 2) - 1);
	data.texture = (packedData >> 4)&((1 << 6) - 1);
	data.lightLevel = (packedData >> 0)&((1 << 4) - 1);

	return data;
}