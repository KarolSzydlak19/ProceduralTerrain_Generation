#include "PerlinNoiseGenerator.h"

// Function to fade the noise
float PerlinNoiseGenerator::fade(float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

// Linear interpolation
float PerlinNoiseGenerator::lerp(float t, float a, float b) {
    return a + t * (b - a);
}

// Gradient function for Perlin noise
float PerlinNoiseGenerator::grad(int hash, float x, float y) {
    int h = hash & 15;  // Convert low 4 bits of hash code
    float u = h < 8 ? x : y;
    float v = h < 4 ? y : x;
    return ((h & 1) ? -u : u) + ((h & 2) ? -v : v);
}

// Basic Perlin Noise function
float PerlinNoiseGenerator::noise(float x, float y) {
    int X = (int)floor(x) & 255;
    int Y = (int)floor(y) & 255;

    x -= floor(x);
    y -= floor(y);

    float u = fade(x);
    float v = fade(y);

    // Hash coordinates of the 4 corners using the permutation vector
    int aa = permutation[X] + Y;
    int ab = permutation[X] + Y + 1;
    int ba = permutation[X + 1] + Y;
    int bb = permutation[X + 1] + Y + 1;

    // Blend results from the 4 corners
    return lerp(v,
        lerp(u, grad(permutation[aa], x, y), grad(permutation[ba], x - 1, y)),
        lerp(u, grad(permutation[ab], x, y - 1), grad(permutation[bb], x - 1, y - 1))
    );
}

// Fractal Perlin Noise (with octaves)
float PerlinNoiseGenerator::fractalNoise(float x, float y, int octaves, float persistence, float frequency, float amplitude) {
    float total = 0.0f;
    float maxValue = 0.0f;  // Used for normalizing the result

    for (int i = 0; i < octaves; i++) {
        // Calculate each octave of noise and sum
        total += noise(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;

        amplitude *= persistence; // Reduce amplitude for next octave
        frequency *= 2.0f;        // Increase frequency for next octave
    }

    return total / maxValue;  // Normalize the result
}