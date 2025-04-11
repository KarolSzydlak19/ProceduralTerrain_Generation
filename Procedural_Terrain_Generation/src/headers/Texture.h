#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include< stb/stb_image.h>
#include "Shader.h"

class Texture {
public:
	GLuint ID;
	GLenum type;
	Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType);
	Texture(unsigned char* data, int width, int height, GLenum format, GLenum texType, GLenum slot, GLenum pixelType);

	void texUnit(Shader& shader, const char* uniform, GLuint uint);
	void Bind();
	void Unbind();
	void Delete();
};
#endif
