#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Map.h"

class MapBuilder {
public:
    std::vector<std::vector<glm::vec3>>& map;
    void Diamond_step(int x, int y, int stepSize, float scale, int size);
    void Square_step(int x, int y, int stepSize, float scale, int size);
    float randomOffset(float range);
    void initAxes(int size);
    void generateVertices(GLfloat* vertices, int size);
    void generateIndices(GLuint* indices, int size);
    MapBuilder(std::vector<std::vector<glm::vec3>>& map);

};
