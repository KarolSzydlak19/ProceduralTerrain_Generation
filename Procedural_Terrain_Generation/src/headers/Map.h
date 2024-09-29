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
#include "PerlinNoiseGenerator.h"
#include <glm/gtc/noise.hpp>
class MapBuilder;
class Map {
private:
	EBO* mapEBO;
	VAO* mapVAO;
	VBO* mapVBO;
	std::vector<std::vector<glm::vec3>>& map;
	float roughness;
	float flatteningScale;
	int size;
	GLfloat* vertices;
	GLuint* indices;
	MapBuilder& mapBuilder;
	std::vector<glm::vec3> tangents;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> normalLines;
	GLuint normalVAO, normalVBO, tangentVBO;
	std::vector<std::vector<float>> noiseMap;
	PerlinNoiseGenerator noiseGen;
	GLuint noiseTexture;
	float maxTextureNoise;
	float minY;
	float maxY;
public:
	float getMinY();
	float getMaxY();
	void searchEdgeValues();
	void setFlattening(float val);
	void setTexNoise(float val);
	void setRoughness(float roughness);
	void setSize(int size);
	void Diamond_step(int x, int y, int stepSize, float scale);
	void Square_step(int x, int y, int stepSize, float scale);
	void generateNoiseMap(int size, float baseFrequency, int octaves, float persistence, float maxNoiseAmplitude);
	void generateWithPerlin();
	void generateNoiseTexture();
	void uploadNoiseTexture(Shader& shader);
	float randomOffset(float range);	//get random value decreasing with reach iteration
	void generate();
	void print();
	void displayMap();
	void generateIndices();
	void generateVertices();
	void printNormals();
	void draw();
	void initAxes();
	void initObjects();
	void cleanUpObjects();
	void generateTangents();
	void generateNormals();
	void generateNormalLines();
	void initNormalObjects();
	void drawNormals();
	Map(int size, float roughness, float flatteningScale, float maxTextureNoise, std::vector<std::vector<glm::vec3>>& map, MapBuilder& mapBuilder, PerlinNoiseGenerator noiseGen);
};