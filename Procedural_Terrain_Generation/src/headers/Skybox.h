#ifndef SKYBOX_H
#define SKYBOX_H
#include <glad/glad.h>
#include <string>
#include <stb_image.h>
#include <iostream>
#include "Shader.h"

class Skybox {
private:

	float skyboxVertices[24];
	unsigned int skyboxIndices[36];
	//	   7----------6
	//	  /|		 /|
	//	 4----------5 |
	//	 | |	    | |
	//	 | 3--------|-2
	//	 |/		    |/
	//	 0----------1	


	unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
	unsigned int cubeMapTexture;
	int textureWidth, textureHeight, nrChannels;
	unsigned char* data;
	Shader& skyboxShader;

public:
	Skybox(Shader& skyboxShader);
	~Skybox();
	void initTexture(const char* image);
	unsigned char* extractFace(unsigned char* data, int faceWidth, int faceHeight, int xOffset, int yOffset);
	void initShader();
	void draw(glm::mat4 view, glm::mat4 projection);
};
#endif
