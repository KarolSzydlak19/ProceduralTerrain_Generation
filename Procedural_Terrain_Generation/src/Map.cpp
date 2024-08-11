#include "Map.h"
#include <Shader.h>

Map::Map(int size, float roughness, std::vector<std::vector<glm::vec3>>& map, MapBuilder& mapBuilder)
    : size(size), roughness(roughness), map(map), mapBuilder(mapBuilder) {
    // Initialize the corners
    map[0][0].y = randomOffset(roughness);
    map[0][size - 1].y = randomOffset(roughness);
    map[size - 1][0].y = randomOffset(roughness);
    map[size - 1][size - 1].y = randomOffset(roughness);
    mapVAO = new VAO();
    mapEBO = nullptr;
    mapVBO = nullptr;
}

float Map::randomOffset(float range) {
    return ((rand() % 100) / 100.0f) * range * 2 - range;
}
void Map::Diamond_step(int x, int y, int stepSize, float scale) {
    mapBuilder.Diamond_step(x, y, stepSize, scale, size);
}

void Map::Square_step(int x, int y, int halfStep, float scale) {
    mapBuilder.Square_step(x, y, halfStep, scale, size);
}

void Map::generate() {
    //print();
    int stepSize = size - 1;
    stepSize /= 2;
    float scale = roughness;
    while (stepSize > 1) {
        int halfStep = stepSize / 2;
        // Diamond step
        for (int x = 0; x < size - 1; x += stepSize) {
            for (int y = 0; y < size - 1; y += stepSize) {
                mapBuilder.Diamond_step(x, y, stepSize, scale, size);
            }
        }
        // Square step
        for (int x = 0; x < size; x += halfStep) {
            for (int y = (x + halfStep) % stepSize; y < size; y += stepSize) {
                mapBuilder.Square_step(x, y, halfStep, scale, size);
            }
        }

        stepSize /= 2;
        scale /= 2.0f;
    }
    //std::cout << "AFTER" << std::endl;
    //print();
}

void Map::initAxes() {
    mapBuilder.initAxes(size);
}

void Map::generateVertices() {
    int vertexCount = size * size;
    vertices = new GLfloat[vertexCount * 5]; // Each vertex has 5 components (x, y, z) + texture coordinates (xTex, yTex)
    mapBuilder.generateVertices(vertices, size);
}

void Map::generateIndices() {
    int indexCount = (size - 1) * (size - 1) * 6; // 6 indices per square (2 triangles)
    indices = new GLuint[indexCount];
    mapBuilder.generateIndices(indices, size);
}

void checkOpenGLError(const std::string& stmt) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << " after " << stmt << std::endl;
    }
}

void Map::initObjects() {
    initAxes();
    generateVertices();
    generateIndices();
    mapVAO->Bind();

    mapVBO = new VBO(vertices, size * size * 5 * sizeof(GLfloat));

    mapEBO = new EBO(indices, (size - 1) * (size - 1) * 6 * sizeof(GLuint));

    mapVAO->LinkAttrib(*mapVBO, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0);

    mapVAO->LinkAttrib(*mapVBO, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    mapVAO->Unbind();
    mapEBO->Unbind();
    mapVBO->Unbind();
}

void Map::draw() {
    int indexCount = (size - 1) * (size - 1) * 6;
    mapVAO->Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    mapVAO->Unbind();

}

void Map::print() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << map[i][j].x << " " << map[i][j].y << " " << map[i][j].z << std::endl;
        }
    }
}