#include "NoiseGenerator.h"

#include <algorithm>

float NoiseGenerator::Normalize(float num)
{
	return std::abs(num) * 2;
}

float NoiseGenerator::GetOctaveNoise(float x, float z, int octaves)
{
	float height = 0;
	float u = 1.0f;

	for (int i = 0; i < octaves; i++) {
		height += GetNoise(x / u, z / u) * u;
		u *= 2.0f;
	}

	return height;
}

float NoiseGenerator::GetCombinedNoise(float x, float z, int octaves1, int octaves2) 
{
	return GetOctaveNoise(x + GetOctaveNoise(x, z, octaves1), z, octaves2);
}
