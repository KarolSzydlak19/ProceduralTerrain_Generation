#ifndef SPHERE_H
#define SPHERE_H
#include <vector>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "EBO.h"
#include "VAO.h"
#include "VBO.h"


class Sphere {
public:
    Sphere(float radius, unsigned int rings, unsigned int sectors);
    ~Sphere();

    void Draw(Shader& shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color);

private:
    void generateVertices(float radius, unsigned int rings, unsigned int sectors);
    void setupMesh();

    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    VAO vao;
    EBO ebo;
    VBO vbo;
};
#endif