#include "Sphere.h"
#include <math.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

Sphere::Sphere(float radius, unsigned int rings, unsigned int sectors) {
    generateVertices(radius, rings, sectors);
    setupMesh();
}

Sphere::~Sphere() {
    vbo.Delete();
    vao.Delete();
    ebo.Delete();
}

void Sphere::generateVertices(float radius, unsigned int rings, unsigned int sectors) {
    const float PI = 3.14159265359f;
    const float TWO_PI = 2.0f * PI;

    for (unsigned int r = 0; r <= rings; ++r) {
        for (unsigned int s = 0; s <= sectors; ++s) {
            float y = sin(-PI / 2.0f + PI * r / rings);
            float x = cos(TWO_PI * s / sectors) * sin(PI * r / rings);
            float z = sin(TWO_PI * s / sectors) * sin(PI * r / rings);

            vertices.push_back(x * radius);
            vertices.push_back(y * radius);
            vertices.push_back(z * radius);
            // +TEXTURE AND NORMALS
        }
    }
    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            unsigned int first = r * (sectors + 1) + s;
            unsigned int second = first + sectors + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}

void Sphere::setupMesh()
{
    vao.Bind();

    vbo = VBO(&vertices[0], vertices.size() * sizeof(float));
    ebo = EBO(&indices[0], indices.size() * sizeof(unsigned int));

    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Sphere::Draw(Shader& shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color)
{
    shader.Activate();

    // Model matrix
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::scale(model, scale);

    vao.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    vao.Unbind();
}