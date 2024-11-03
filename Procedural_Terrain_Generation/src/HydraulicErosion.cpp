#include "HydraulicErosion.h"

HydraulicErosion::HydraulicErosion(std::vector<std::vector<glm::vec3>>& heightMap)
	: heightMap(heightMap) {
}

void HydraulicErosion::setWaterAdditionRate(float x) {
	waterAdditionRate = x;
}

void HydraulicErosion::setSedimentCapacity(float x) {
	sedimentCapacity = x;
}

void HydraulicErosion::setEvaporationRate(float x) {
	evaporationRate = x;
}

void HydraulicErosion::setDepositionRate(float x) {
	depositionRate = x;
}

void HydraulicErosion::setErosionRate(float x) {
	erosionRate = x;
}

void HydraulicErosion::setMapSize(int x) {
	mapSize = x;
}

int HydraulicErosion::randomInt(int range) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(0, range - 1);
	return distr(gen);
}

float HydraulicErosion::randomOffset(float range) {
	float offset = ((rand() % 100) / 100.0f) * range * 2 - range;
	return offset;	
}
glm::vec3 HydraulicErosion::calculateNormal(int x, int y) {
	glm::vec3 normal = glm::vec3(0.15) * glm::normalize(heightMap[x][y] - heightMap[x + 1][y]);
	normal += glm::vec3(0.15f) * glm::normalize(heightMap[x - 1][y] - heightMap[x][y]);
	normal += glm::vec3(0.15f) * glm::normalize(heightMap[x][y] - heightMap[x][y + 1]);
	normal += glm::vec3(0.15f) * glm::normalize(heightMap[x][y - 1] - heightMap[x][y]);

	//diagonals
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x + 1][y + 1]) / glm::vec3(sqrt(2)));
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x + 1][y - 1]) / glm::vec3(sqrt(2)));
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x - 1][y - 1]) / glm::vec3(sqrt(2)));
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x - 1][y + 1]) / glm::vec3(sqrt(2)));
	return normal;
}

void HydraulicErosion::Erode(int iterations, float volume, float dt, float density, float friction) {
	float progressPerIteration = 1 / ((float)iterations);
	int range = 50.0f;
	for (int i = 0; i < iterations; i++) {
		glm::vec2 newPos;
		do {
			newPos = glm::vec2(randomOffset(mapSize - 1), randomOffset(mapSize - 1));
		} while ((int)newPos.x < 1 || (int)newPos.x  > mapSize - 2 || (int)newPos.y < 1 || (int)newPos.y > mapSize - 2);
		glm::vec2 speed = { 0.0f, 0.0f };
		float sediment = 0.0f;
		//std::cout << volume << std::endl;
	    std::cout << i << std::endl;
		//std::cout << "-------" << std::endl;
		float currentVolume = volume;
		while (currentVolume > 0.0000001) {
			glm::ivec2 iPos = newPos;
			glm::vec3 normal = calculateNormal(iPos.x, iPos.y);
			//glm::vec2 gradient = glm::vec2(heightMap[iPos.x + 1][iPos.y].y - heightMap[iPos.x - 1][iPos.y].y,
			//	heightMap[iPos.x][iPos.y + 1].y - heightMap[iPos.x][iPos.y - 1].y);
			//gradient = glm::normalize(gradient);

			speed += dt * glm::vec2(normal.x, normal.y) / (currentVolume * density);
			/*if (speed.x > 0.0f) {
				newPos.x += 1.0f;
			}
			else if (speed.x < 0.0f) {
				newPos.x -= 1.0f;
			}
			if (speed.y > 0.0f) {
				newPos.y += 1.0f;
			} 
			else if (speed.y < 0.0f) {
				newPos.y -= 1.0f;
			}*/
			newPos += dt * speed;
			speed *= (1.0 - dt * friction);


			if (newPos.x < 1 || newPos.x > mapSize - 2 || newPos.y < 1 || newPos.y > mapSize - 2) {
				break;
			}
			//std::cout << "after speed and pos" << std::endl;
			float maxSediment = currentVolume * glm::length(speed) * (heightMap[iPos.x][iPos.y].y - heightMap[(int)newPos.x][(int)newPos.y].y);
			if (maxSediment < 0.0f) maxSediment = 0.0f;
			float diff = maxSediment - sediment;
			//sediment += dt * depositionRate;
			//std::cout << "?" << std::endl;
			sediment += dt* erosionRate * diff;
			
			//std::cout << "??" << std::endl;
			float removal = dt * currentVolume * diff * erosionRate;
			heightMap[iPos.x][iPos.y].y -= removal;
			
			// range
			//std::cout << "ramge" << std::endl;
			/*int k = iPos.x - range;
			if (k < 0) {
				k = 0;
			}
			else if (k > mapSize - 1)
			{
				k = mapSize - 1;
			}
			int l = iPos.y - range;
			if (l < 0) {
				l = 0;
			}
			else if (l > mapSize - 1) {
				l = mapSize - 1;
			}*/

			//for (int n = k; n < iPos.x; n++) {
			//	for (int m = l; m < iPos.y; m++) {
			//		float distance = ((iPos.x - n) ^ 2 + (iPos.y - m) ^ 2) / sqrt(2);
			//		if (distance != 0)
			//		heightMap[n][iPos.y].y -= removal / (((iPos.x - n) ^ 2 + (iPos.y - m) ^ 2) / sqrt(2));
			//		//std::cout << "Mianownik " <<  removal / (((iPos.x - n) ^ 2 + (iPos.y - m) ^ 2) / sqrt(2)) << std::endl;
			//	}
			//}
			/*for (int n = iPos.x + 1; n < iPos.x + k; n++) {
				for (int m = iPos.y; m < iPos.y + l; m++) {
					float distance = ((iPos.x - n) ^ 2 + (iPos.y - m) ^ 2) / sqrt(2);
					if (distance != 0)
					heightMap[n][iPos.y].y -= removal / (((iPos.x - n) ^ 2 + (iPos.y - m) ^ 2) / sqrt(2));
				}
			}*/
			currentVolume *= (1.0 - dt * evaporationRate);
			
		}
	}
}

void HydraulicErosion::ErodeWholeMap(int iterations, float volume, float dt, float density, float friction, glm::ivec2 start) {
	float progressPerIteration = 1 / ((float)iterations);
	int range = 50.0f;
	//for (int i = 0; i < iterations; i++) {
		glm::vec2 newPos = start;
		glm::vec2 speed = { 0.0f, 0.0f };
		float sediment = 0.0f;
		float currentVolume = volume;
		//std::cout << start.x << " " << start.y << std::endl;
		while (currentVolume > 0.000000001) {
			glm::ivec2 iPos = newPos;
			glm::vec3 normal = calculateNormal(iPos.x, iPos.y);
			//glm::vec2 gradient = glm::vec2(heightMap[iPos.x + 1][iPos.y].y - heightMap[iPos.x - 1][iPos.y].y,
			//	heightMap[iPos.x][iPos.y + 1].y - heightMap[iPos.x][iPos.y - 1].y);
			//gradient = glm::normalize(gradient);

			speed += dt * glm::vec2(normal.x, normal.y) / (currentVolume * density);
			/*if (speed.x > 0.0f) {
				newPos.x += 1.0f;
			}
			else if (speed.x < 0.0f) {
				newPos.x -= 1.0f;
			}
			if (speed.y > 0.0f) {
				newPos.y += 1.0f;
			}
			else if (speed.y < 0.0f) {
				newPos.y -= 1.0f;
			}*/
			newPos += dt * speed;
			speed *= (1.0 - dt * friction);


			if (newPos.x < 1 || newPos.x > mapSize - 2 || newPos.y < 1 || newPos.y > mapSize - 2) {
				break;
			}
			//std::cout << "after speed and pos" << std::endl;
			float maxSediment = currentVolume * glm::length(speed) * (heightMap[iPos.x][iPos.y].y - heightMap[(int)newPos.x][(int)newPos.y].y);
			if (maxSediment < 0.0f) maxSediment = 0.0f;
			float diff = maxSediment - sediment;
			sediment += dt * erosionRate * diff;


			float removal = dt * currentVolume * diff * erosionRate;
			//if (removal > 1.0f) removal = 1.0f;
			if (removal > 500.0f) {
				std::cout << "Removal: " << removal  << " speed.x: " << speed.x << " speed.y: " << speed.y << std::endl;
			}
			heightMap[iPos.x][iPos.y].y -= removal;

			//std::cout << iPos.x << " " << iPos.y << " " << removal << std::endl;
			currentVolume *= (1.0 - dt * evaporationRate);

		}
	//}
}
