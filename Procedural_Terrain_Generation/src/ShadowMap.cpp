#include "ShadowMap.h"
#include <iostream>

ShadowMap::ShadowMap(GLuint width, GLuint height) : shadowWidth(width), shadowHeight(height) {
    Init();
}

void ShadowMap::Init() {
    // Generate framebuffer for shadow map
    glGenFramebuffers(1, &depthMapFBO);

    // Generate texture to store depth values
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Set border color to white (optional)
    GLfloat borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    // Attach depth texture to framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

    // No color buffer is drawn to
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    // Check framebuffer completeness
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Framebuffer not complete!" << std::endl;
    }

    // Unbind the framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "!!!!!!!!!!!!!!!!Framebuffer not complete!" << std::endl;
    }

    std::cout << "shdowMapText ID :" << depthMap << std::endl;
    std::cout << "shmFBO: " << depthMapFBO << std::endl;
}

void ShadowMap::BindForWriting() {
    // Bind the shadow map framebuffer
    glEnable(GL_DEPTH_TEST);
    glViewport(0, 0, shadowWidth, shadowHeight); // Set the viewport to the shadow map dimensions
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT); // Clear the depth buffer
}

void ShadowMap::Unbind() {
    // Unbind the framebuffer, restoring the default one
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1920, 1080); // Optional: reset viewport to your default window dimensions
}

void ShadowMap::BindForReading(GLenum textureUnit) {
    // Bind the depth map texture to a texture unit
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, depthMap);
}

GLuint ShadowMap::getDepthMapTexture() const {
    return depthMap;
}

GLuint ShadowMap::getFBO() const {
    return depthMapFBO;
}
