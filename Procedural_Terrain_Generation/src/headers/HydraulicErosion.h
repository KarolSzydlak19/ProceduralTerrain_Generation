#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <iostream>
#include <random>
#include <cmath>
#include "Drop.h"

class HydraulicErosion {
private:
	struct HeightAndGradient {
		 float height;
		 float gradientX;
		 float gradientY;
	};

	int mapSize;
	float waterAdditionRate;
	float sedimentCapacity;
	float evaporationRate;
	float depositionRate;
	float erosionRate;
	std::vector<std::vector<float>> waterMap;
	std::vector<std::vector<float>> sedimentMap;
	std::vector<std::vector<glm::vec3>>& heightMap;
public:
	//setters
	void setWaterAdditionRate(float x);
	void setSedimentCapacity(float x);
	void setEvaporationRate(float x);
	void setDepositionRate(float x);
	void setErosionRate(float x);
	void setMapSize(int x);
	int randomInt(int range);
	HydraulicErosion(std::vector<std::vector<glm::vec3>>& heightMap);
	void Erode(int iterations, float volume, float dt, float density, float friction, glm::vec2 start);
	void ErodeWholeMap(int iterations, float volume, float dt, float density, float friction, glm::ivec2 start);
	glm::vec3 calculateNormal(int x, int y);
	HeightAndGradient calculateDirection(int x, int y);
	void performErosion(int iterations);
	void addWater();
	void simulateWaterFlow(int iteration);
	void evaporateWater();
	float randomOffset(float range);
};