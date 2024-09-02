#pragma once
#include <glm/fwd.hpp>
#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include <vector>
#include <EBO.h>
#include <VAO.h>
#include <Shader.h>
#include "Texture.h"
#include "MapBuilder.h"
class MapBuilder;
class Map {
private:
	EBO* mapEBO;
	VAO* mapVAO;
	VBO* mapVBO;
	std::vector<std::vector<glm::vec3>>& map;
	float roughness;
	int size;
	GLfloat* vertices;
	GLuint* indices;
	MapBuilder& mapBuilder;
	std::vector<glm::vec3> tangents;
public:

	void Diamond_step(int x, int y, int stepSize, float scale);
	void Square_step(int x, int y, int stepSize, float scale);
	float randomOffset(float range);	//get random value decreasing with reach iteration
	void generate();
	void print();
	void displayMap();
	void generateIndices();
	void generateVertices();
	void draw();
	void initAxes();
	void initObjects();
	void generateTangents();
	Map(int size, float roughness, std::vector<std::vector<glm::vec3>>& map, MapBuilder& mapBuilder);
};