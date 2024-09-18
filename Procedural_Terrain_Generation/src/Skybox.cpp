#include "Skybox.h"
#include <gtc/type_ptr.hpp>

Skybox::Skybox(Shader& skyboxShader) :
    skyboxShader(skyboxShader)
{
    float vertices[] = {
        -1.0f, -1.0f, 1.0f,  
        1.0f, -1.0f, 1.0f,    
        1.0f, -1.0f, -1.0f,  
        -1.0f, -1.0f, -1.0f, 
        -1.0f, 1.0f, 1.0f,    
        1.0f, 1.0f, 1.0f,
        1.0f, 1.0f, -1.0f,
        -1.0f, 1.0f, -1.0f,   
    };

    unsigned int indices[] = {
        // Right
        1, 2, 6,
        6, 5, 1,
        // Left
        0, 4, 7,
        7, 3, 0,
        // Top
        4, 5, 6,
        6, 7, 4,
        // Bottom
        0, 3, 2,
        2, 1, 0,
        // Back
        0, 1, 5,
        5, 4, 0,
        // Front
        3, 7, 6,
        6, 2, 3
    };

    // Copy data into class members
    memcpy(skyboxVertices, vertices, sizeof(vertices));
    memcpy(skyboxIndices, indices, sizeof(indices));

    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    initShader();
    std::cout << "IN SKYBOX --> " << skyboxShader.ID << std::endl;
    int index = 0;
    for (int i = 0; i < 8; i++) {
        std::cout << skyboxVertices[index] << " " << skyboxVertices[index + 1] << " " << skyboxVertices[index + 2] << std::endl;
        index += 3;
    }
}

Skybox::~Skybox() {
    if (data) {
        //stbi_image_free(data);
    }
}

void Skybox::initTexture(const char* image) {
    std::cout << "SKYBOX INITIALIZATION" << std::endl;
    glGenTextures(1, &cubeMapTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    data = stbi_load(image, &textureWidth, &textureHeight, &nrChannels, 0);
    if (data) {
        int faceWidth = textureWidth / 4;
        int faceHeight = textureHeight / 3;
        stbi_set_flip_vertically_on_load(false);

        // Right face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, extractFace(data, faceWidth, faceHeight, 2 * faceWidth, faceHeight));

        // Left face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, extractFace(data, faceWidth, faceHeight, 0, faceHeight));

        // Top face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, extractFace(data, faceWidth, faceHeight, faceWidth, 0));

        // Bottom face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, extractFace(data, faceWidth, faceHeight, faceWidth, 2 * faceHeight));

        // Front face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, extractFace(data, faceWidth, faceHeight, faceWidth, faceHeight));

        // Back face
        glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, faceWidth, faceHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, extractFace(data, faceWidth, faceHeight, 3 * faceWidth, faceHeight));
        stbi_image_free(data);
        std::cerr << "Texture loaded " << image << std::endl;

    }
    else {
        std::cerr << "####Failed to load texture at:#### " << image << std::endl;
    }
}

unsigned char* Skybox::extractFace(unsigned char* data, int faceWidth, int faceHeight, int xOffset, int yOffset) {
    std::cout << "SKYBOX TEXTURE EXTRACTION" << std::endl;
    unsigned char* subTexture = new unsigned char[faceWidth * faceHeight * nrChannels];

    // Loop through each row of the face and copy the correct portion of the data
    for (int row = 0; row < faceHeight; row++) {
        memcpy(
            subTexture + row * faceWidth * nrChannels,
            data + ((yOffset + row) * textureWidth + xOffset) * nrChannels,
            faceWidth * nrChannels
        );
    }

    return subTexture;
}

void Skybox::draw(glm::mat4 view, glm::mat4 projection) {
    glDepthFunc(GL_LEQUAL);
    skyboxShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    glDepthFunc(GL_LESS);
}

void Skybox::initShader() {
    skyboxShader.Activate();
    glUniform1i(glGetUniformLocation(skyboxShader.ID, "skybox"), 7);
}
