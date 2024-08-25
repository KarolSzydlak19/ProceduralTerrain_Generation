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
            // Normal vector (same as position but normalized, since it's a unit sphere)
           // vertices.push_back(x);
            //vertices.push_back(y);
            //vertices.push_back(z);

            // Texture coordinates
            float u = 0.25f + 0.5f * (float)s / (float)sectors;  // u coordinate
            float v = 0.25f + 0.5f * (float)r / (float)rings;    // v coordinate
            vertices.push_back(u);
            vertices.push_back(v);
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

    //vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);
    //Position attributes
    vao.LinkAttrib(vbo, 0, 3, GL_FLOAT, 5 * sizeof(float), (void*)0);
    //Texture coordinates
    vao.LinkAttrib(vbo, 1, 2, GL_FLOAT, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    vao.Unbind();
    vbo.Unbind();
    ebo.Unbind();
}

void Sphere::Draw(Shader& shader, glm::vec3 position, glm::vec3 scale, glm::vec3 color)
{
    shader.Activate();
    vao.Bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    vao.Unbind();
}
// Translation of sphere, works only if indices contain positions + 2 coordinates for textures
// if that structure is modified so should this function be
void Sphere::translate(glm::vec3 vec) {
    model = glm::translate(model, vec);
}