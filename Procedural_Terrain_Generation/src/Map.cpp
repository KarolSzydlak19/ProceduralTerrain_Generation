
#include "Map.h"
#include <Shader.h>

Map::Map(int size, float roughness, std::vector<std::vector<glm::vec3>>& map)
    : size(size), roughness(roughness), map(map) {

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
    stepSize /= 2;
    float scale = roughness;
    while (stepSize > 1) {
        int halfStep = stepSize / 2;
        // Diamond step
        for (int x = 0; x < size - 1; x += stepSize) {
            for (int y = 0; y < size - 1; y += stepSize) {
                Diamond_step(x, y, stepSize, scale);
            }
        }
        // Square step
        for (int x = 0; x < size; x += halfStep) {
            for (int y = (x + halfStep) % stepSize; y < size; y += stepSize) {
                Square_step(x, y, halfStep, scale);
            }
        }

        stepSize /= 2;
        scale /= 2.0f;
    }
}

void Map::initAxes() {
    float x = 0.0f;
    for (int i = 0; i < size; i++) {
        x += 0.05;
        float z = 0;
        for (int j = 0; j < size; j++) {
            map[i][j].x = x;
        }
    }
    float y = 0.0f;
    for (int i = 0; i < size; i++) {
        y += 0.05;
        for (int j = 0; j < size; j++) {
            map[j][i].z = y;
        }
    }
    std::cout << "Init axes" << std::endl;
}

void Map::generateVertices() {
    int vertexCount = size * size;
    float xTexBase = 1.0f / size;
    float yTexBase = 1.0f / size;
    float xTex = 0.0f;
    float yTex = 0.0f;
    vertices = new GLfloat[vertexCount * 5]; // Each vertex has 5 components (x, y, z) + texture coordinates (xTex, yTex)
    int index = 0;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            vertices[index++] = map[i][j].x;
            vertices[index++] = map[i][j].y;
            vertices[index++] = map[i][j].z;

            vertices[index++] = xTex;
            vertices[index++] = yTex;
            //std::cout << "xTex: " << " " << xTex << " " << "yTex" << yTex <<  std::endl;
            xTex += xTexBase;
        }
        xTex = 0.0f;
        yTex += yTexBase;
    }
    std::cout << "vertices:" << std::endl;
    for (int i = 0; i < vertexCount * 3; i += 3) {
        //std::cout << vertices[i] << " " << vertices[i+ 1] << " " << vertices[i + 2] << std::endl;
    }
}

void Map::generateIndices() {
    int indexCount = (size - 1) * (size - 1) * 6; // 6 indices per square (2 triangles)
    indices = new GLuint[indexCount];
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
    std::cout << "Indices:" << std::endl;
    for (int i = 0; i < indexCount; i++) {
        //std::cout << i << " " << indices[i] << std::endl;
    }
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
