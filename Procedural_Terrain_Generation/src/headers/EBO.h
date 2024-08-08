#ifndef EBO_CLASS_H
#define EBO_CLASS_H

#include<glad/glad.h>
#include<vector>
#include<glm/glm.hpp>
#pragma once
class EBO
{
public:
	GLuint id;
	EBO(GLuint* indices, GLsizeiptr size);
	EBO();

	void Bind();
	void Unbind();
	void Delete();
};
#endif
