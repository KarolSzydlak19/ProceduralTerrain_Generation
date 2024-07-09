
#include "Map.h"

Map::Map(int size, float roughness, std::vector<std::vector<glm::vec3>>& map)
    : size(size), roughness(roughness), map(map) {

    // Initialize the corners
    map[0][0].y = randomOffset(roughness);
    map[0][size - 1].y = randomOffset(roughness);
    map[size - 1][0].y = randomOffset(roughness);
    map[size - 1][size - 1].y = randomOffset(roughness);
}

float Map::randomOffset(float range) {
    return ((rand() % 100) / 100.0f) * range * 2 - range;
}
void Map::Diamond_step(int x, int y, int stepSize, float scale) {
    int halfStep = stepSize / 2;
    float avg = (map[x][y].y + map[x + stepSize][y].y +
        map[x][y + stepSize].y + map[x + stepSize][y + stepSize].y) / 4.0f;
    map[x + halfStep][y + halfStep].y = avg + randomOffset(scale);
}

void Map::Square_step(int x, int y, int halfStep, float scale) {

    float avg = 0.0f;
    int count = 0;

    if (y >= halfStep) {
        avg += map[x][y - halfStep].y;
        count++;
    }
    if (y + halfStep < size) {
        avg += map[x][y + halfStep].y;
        count++;
    }
    if (x >= halfStep) {
        avg += map[x - halfStep][y].y;
        count++;
    }
    if (x + halfStep < size) {
        avg += map[x + halfStep][y].y;
        count++;
    }

    map[x][y].y = avg / count + randomOffset(scale);
}
void Map::generate() {
    int stepSize = size - 1;
    float scale = roughness;

    while (stepSize > 1) {
        // Diamond step
        for (int x = 0; x < size - 1; x += stepSize) {
            for (int y = 0; y < size - 1; y += stepSize) {
                Diamond_step(x, y, stepSize, scale);
            }
        }

        // Square step
        int halfStep = stepSize / 2;
        for (int x = 0; x < size; x += halfStep) {
            for (int y = (x + halfStep) % stepSize; y < size; y += stepSize) {
                Square_step(x, y, halfStep, scale);
            }
        }

        stepSize /= 2;
        scale /= 2.0f;
    }
}
void Map::print() {
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            std::cout << map[i][j].x << " " << map[i][j].y << " " << map[i][j].z << std::endl;
        }
    }
}