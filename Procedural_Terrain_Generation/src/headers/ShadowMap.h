#ifndef SHADOWMAP_H
#define SHADOWMAP_H
#include <glad/glad.h>
#include <glm/glm.hpp>

class ShadowMap {
public:
	GLuint shadowWidth, shadowHeight;

	ShadowMap(GLuint width, GLuint height);
	// Set up the depth map and framebuffer
	void Init();

	// Bind the shadow framebuffer for rendering
	void BindForWriting();

	// Bind the depth texture for sampling
	void Unbind();

	// Bind depth texture for sampling
	void BindForReading(GLenum textureUnit);

	GLuint getDepthMapTexture() const;

	GLuint getFBO() const;

private:
	GLuint depthMapFBO; // Framebuffer object for shadow map
	GLuint depthMap;    // Depth map texture
};
#endif // !SHADOWMAP_H