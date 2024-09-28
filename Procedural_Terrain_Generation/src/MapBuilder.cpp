#include "MapBuilder.h"
#include <cstdlib>

MapBuilder::MapBuilder(std::vector<std::vector<glm::vec3>>& map, float meshOffset) :
    map(map), meshOffset(meshOffset) {
    maxY = minY = 0;
}

float MapBuilder::randomOffset(float range) {
    float offset =  ((rand() % 100) / 100.0f) * range * 2 - range;
    //std::cout <<"Range " << range << " Random Offset: " << offset << std::endl;
    return offset;
}

float MapBuilder::getMinY() {
    return minY;
}

float MapBuilder ::getMaxY() {
    return maxY;
}

void MapBuilder::setOffset(float offset) {
    meshOffset = offset;
}

void MapBuilder::Diamond_step(int x, int y, int stepSize, float scale, int size) {
    int halfStep = stepSize / 2;
    float avg = 0.0f;
    int count = 0;

    avg += map[x][y].y;
    count++;

    if (x + stepSize < size) {
        avg += map[x + stepSize][y].y;
        count++;
    }
    if (y + stepSize < size) {
        avg += map[x][y + stepSize].y;
        count++;
    }
    if (x + stepSize < size && y + stepSize < size) {
        avg += map[x + stepSize][y + stepSize].y;
        count++;
    }

    avg = avg / count;
    map[x + halfStep][y + halfStep].y = avg + randomOffset(scale);
}

void MapBuilder::Square_step(int x, int y, int halfStep, float scale, int size) {
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

void MapBuilder::initAxes(int size) {
    float x = 0.0f;
    for (int i = 0; i < size; i++) {
        //x += 0.05;
        x += meshOffset;
        float z = 0;
        for (int j = 0; j < size; j++) {
            map[i][j].x = x;
        }
    }
    float y = 0.0f;
    for (int i = 0; i < size; i++) {
        //y += 0.05;
        y += meshOffset;
        for (int j = 0; j < size; j++) {
            map[j][i].z = y;
        }
    }
    std::cout << "Init axes" << std::endl;
}

void MapBuilder::generateVertices(GLfloat* vertices, int size) {
    int vertexCount = size * size;
    float xTexBase = 2.0f / size;
    float yTexBase = 2.0f / size;
    //float xTexBase = 0.0001f;
    //float yTexBase = 0.0001f;
    float xTex = 0.0f;
    float yTex = 0.0f;
    int index = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            vertices[index++] = map[i][j].x;
            vertices[index++] = map[i][j].y;
            vertices[index++] = map[i][j].z;

            vertices[index++] = xTex;
            vertices[index++] = yTex;
            xTex += xTexBase;
        }
        xTex = 0.0f;
        yTex += yTexBase;
    }
}

void MapBuilder::generateIndices(GLuint* indices, int size) {
    int indexCount = (size - 1) * (size - 1) * 6; // 6 indices per square (2 triangles)
    int index = 0;
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - 1; ++j) {
            indices[index++] = i * size + j;
            indices[index++] = i * size + (j + 1);
            indices[index++] = (i + 1) * size + j;

            indices[index++] = i * size + (j + 1);
            indices[index++] = (i + 1) * size + (j + 1);
            indices[index++] = (i + 1) * size + j;
        }
    }
}
