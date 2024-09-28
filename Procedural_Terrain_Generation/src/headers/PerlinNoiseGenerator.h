#pragma once
#include <cmath>
#include <vector>

class PerlinNoiseGenerator {
public:
	float fade(float t);
	float lerp(float t, float a, float b);
	float grad(int hash, float x, float y);
	float perlin(float x, float y);
	float fractalNoise(float x, float y, int octaves, float persistance, float frequency, float amplitude);
};