#include "FastNoise.h"

class NoiseGenerator : public FastNoise
{
public:
	float Normalize(float num);
	float GetOctaveNoise(float x, float z, int octaves);
	float GetCombinedNoise(float x, float z, int octaves1, int octaves2);
};