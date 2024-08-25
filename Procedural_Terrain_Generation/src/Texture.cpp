#include "Texture.h"

Texture::Texture(const char* image, GLenum texType, GLenum slot, GLenum format, GLenum pixelType) {
	type = texType;

	int width, height, numColCh;
	
	stbi_set_flip_vertically_on_load(true);

	unsigned char* bytes = stbi_load(image, &width, &height, &numColCh, 0);

	if (!bytes) {
		std::cerr << "Failed to load texture: " << image << std::endl;
		return;
	}

	glGenTextures(1, &ID);

	glActiveTexture(slot);
	glBindTexture(texType, ID);

	glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexImage2D(texType, 0, GL_RGBA, width, height, 0, format, pixelType, bytes);
	
	glGenerateMipmap(texType);
	stbi_image_free(bytes);

	glBindTexture(texType, 0);
}

Texture::Texture(unsigned char* data, int width, int height, GLenum format, GLenum texType, GLenum slot, GLenum pixelType)
{
	type = texType;

	// Generate a texture ID
	glGenTextures(1, &ID);
	glActiveTexture(slot);
	glBindTexture(type, ID);

	// Specify texture parameters
	glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Upload texture data
	glTexImage2D(type, 0, format, width, height, 0, format, pixelType, data);

	// Generate Mipmaps (optional)
	glGenerateMipmap(type);

	// Unbind the texture
	glBindTexture(type, 0);
}

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{
	
	GLuint texUni = glGetUniformLocation(shader.ID, uniform);
	
	shader.Activate();
	
	glUniform1i(texUni, unit);
}

void Texture::Bind()
{
	glBindTexture(type, ID);
}

void Texture::Unbind()
{
	glBindTexture(type, 0);
}

void Texture::Delete()
{
	glDeleteTextures(1, &ID);
}