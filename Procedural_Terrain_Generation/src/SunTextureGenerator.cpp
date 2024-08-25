#include <vector>
#include <cmath>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

Texture generateSunTexture(int width, int height)
{
    std::vector<unsigned char> data(width * height * 3);

    float centerX = width / 2.0f;
    float centerY = height / 2.0f;
    float maxDistance = std::sqrt(centerX * centerX + centerY * centerY);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            float dx = x - centerX;
            float dy = y - centerY;
            float distance = std::sqrt(dx * dx + dy * dy) / maxDistance;

            // Calculate color based on distance
            float intensity = 1.0f - distance;
            unsigned char r = static_cast<unsigned char>(255 * intensity);
            unsigned char g = static_cast<unsigned char>(200 * intensity);
            unsigned char b = static_cast<unsigned char>(100 * intensity);

            int offset = (y * width + x) * 3;
            data[offset] = r;
            data[offset + 1] = g;
            data[offset + 2] = b;
        }
    }

    // Create a Texture object using the generated data
    Texture sunTexture(data.data(), width, height, GL_RGB, GL_TEXTURE_2D, GL_TEXTURE0, GL_UNSIGNED_BYTE);

    return sunTexture;
}
