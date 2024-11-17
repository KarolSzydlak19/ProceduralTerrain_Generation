#include "HydraulicErosion.h"
const float gravity = 9.81f;

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
	glm::vec3 normal = glm::vec3(0.0);
	if (x + 1 <= mapSize - 1)
	normal += glm::vec3(0.15) * glm::normalize(heightMap[x][y] - heightMap[x + 1][y]);
	if (x - 1  >= 0)
	normal += glm::vec3(0.15f) * glm::normalize(heightMap[x - 1][y] - heightMap[x][y]);
	if (y + 1 <= mapSize - 1)
	normal += glm::vec3(0.15f) * glm::normalize(heightMap[x][y] - heightMap[x][y + 1]);
	if (y - 1 >= 0)
	normal += glm::vec3(0.15f) * glm::normalize(heightMap[x][y - 1] - heightMap[x][y]);

	//diagonals
	if (x + 1 <= mapSize - 1 && y + 1 <= mapSize - 1)
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x + 1][y + 1]) / glm::vec3(sqrt(2)));
	if (x + 1 <= mapSize - 1 && y - 1 >= 0)
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x + 1][y - 1]) / glm::vec3(sqrt(2)));
	if (x - 1 >= 0 && y - 1 >= 0)
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x - 1][y - 1]) / glm::vec3(sqrt(2)));
	if (x - 1 >= 0 && y + 1 <= mapSize - 1)
	normal += glm::vec3(0.1f) * glm::normalize((heightMap[x][y] - heightMap[x - 1][y + 1]) / glm::vec3(sqrt(2)));
	return normal;
}

void HydraulicErosion::Erode(int iterations, float volume, float dt, float density, float friction, glm::vec2 start) {
	glm::vec2 newPos = start;
	glm::vec3 speed = { 0.0f, 0.0f, 0.0f };
	float sediment = 0.0f;
	int deposited = 0;
	float currentVolume = volume;
	float Scap = sedimentCapacity;
	//glm::vec3 gravity = glm::normalize(glm::vec3(1.0f, -1.0f, 1.0f)) * 9.81f;
	newPos.x = randomInt(mapSize - 1);
	newPos.y = randomInt(mapSize - 1);
	while (currentVolume > 0.000000001) {
		glm::ivec2 iPos = newPos;
		float lowestN = std::numeric_limits<float>::max();
		glm::ivec2 lowestNPos = { 0.0f, 0.0f };

		//lowest neighbour calculation
		for (int k = iPos.x - 1; k < iPos.x + 2; k++) {
			for (int l = iPos.y - 1; l < iPos.y + 2; l++) {
				if (k == iPos.x && l == iPos.y) continue;
				if (k > 0 && l > 0 && k < mapSize - 1 && l < mapSize - 1) {
					if (heightMap[k][l].y < lowestN) {
						lowestNPos.x = k;
						lowestNPos.y = l;
					}
				}
			}
		}
		float heightDiff = heightMap[iPos.x][iPos.y].y - heightMap[lowestNPos.x][lowestNPos.y].y;
		float sinA = 0;
		glm::vec3 X = heightMap[iPos.x][iPos.y] - heightMap[lowestNPos.x][lowestNPos.y];
		if (glm::length(X) != 0) {
			sinA = heightDiff / glm::length(X);
		}
		glm::vec3 acc = glm::normalize(X) * gravity * sinA;

		//std::cout << "spped " << speed.x << "  " << speed.y << " " << speed.z << ::std::endl;
		//std::cout << "acc " << acc.x << "  " << acc.y << " " << acc.z << ::std::endl;
		//std::cout << "acc " << acc.x << " " << acc.y << " "<<acc.z <<std::endl;
		//std::cout << "sina " << sinA << std::endl;
		//speed += acc * dt / (currentVolume * density);
		
		glm::vec2 speed2D = glm::vec2(speed.x, speed.z);
		//std::cout << "SPEED2D " << speed2D.x << " " << speed2D.y << std::endl;
		//std::cout << "abs " << std::abs(speed.y) << "length " << glm::length(speed) << std::endl;
		//float verticalFactor = 1.0f - std::abs(speed.y) / glm::length(speed);
		//speed2D *= verticalFactor;
		speed2D.x = speed.x;
		speed2D.y = speed.z;
		if (glm::length(speed2D) != 0.0f) {
			newPos += speed2D;//glm::normalize(speed2D);
		}
		//std::cout << "Before check" << std::endl;
		//std::cout << newPos.x << " " << newPos.y << std::endl;
		if ((int)newPos.x < 0 || (int)newPos.x > mapSize - 2 || (int)newPos.y < 0 || (int)newPos.y > mapSize - 2) {
			break;
		}
		//std::cout << "AfterCHECK" << std::endl;
		//float maxSediment = currentVolume * glm::length(speed);
		//if (maxSediment < 0.0f) maxSediment = 0.0f;
		//float diff = maxSediment - sediment;
		//std::cout << "Przed deposit" << std::endl;
		//std::cout << "speed " << glm::length(speed) << std::endl;
		//std::cout << "iPos " << iPos.x << " " << iPos.y << std::endl;
		//std::cout << "newPos " << newPos.x << " " << newPos.y << std::endl;
		//std::cout << "neighb " << lowestNPos.x << " " << lowestNPos.y << std::endl;
		//std::cout << "CurrentPNo(int) " << heightMap[iPos.x][iPos.y].y << std::endl;
		//std::cout << "CurrentP " << heightMap[(int)iPos.x][(int)iPos.y].y << " neigb " << heightMap[lowestNPos.x][lowestNPos.y].y << std::endl;
		Scap = sedimentCapacity * currentVolume * glm::length(speed);
		//std::cout << Scap << std::endl;
		if (sediment < Scap) {
			float removal = dt * erosionRate * (Scap - sediment);
			if (removal > heightDiff* erosionRate) removal = heightDiff * erosionRate;
			//std::cout << "Removal:  " << removal << std::endl;
			sediment += removal;
			heightMap[iPos.x][iPos.y].y -= removal;
			if (removal > 100 || removal < - 100) {
				//std::cout << "removal " <<  removal <<" heightDiff " << heightDiff << " sediment " << sediment << "scap " << Scap << glm::length(speed) << std::endl;
			}
		}
		else if (sediment > Scap) {
			float depos = dt * depositionRate * (sediment - Scap);
			if (depos > heightDiff * depositionRate) { 
				depos = heightDiff * depositionRate;
				//sediment -= heightDiff;
			}
			sediment -= depos;
			//std::cout << "deposit " << depos << std::endl;
			heightMap[iPos.x][iPos.y].y += depos;
			if (sediment < 0.0f) sediment = 0.0f;
			if (depos > 100 || depos < -100) {
				//std::cout <<"depos " << depos  << " heightDiff " << heightDiff << " sediment " << sediment << "scap " << Scap << "speed " << glm::length(speed) << std::endl;
			}
		}
		//std::cout << "PO deposit" << std::endl;
		currentVolume *= (1.0 - dt * evaporationRate);
		speed = speed * (1 - friction) + acc * dt;
		/*float removal = dt * currentVolume * diff * erosionRate;
		sediment += removal * depositionRate;
		if (!std::isnan(removal) && !std::isinf(removal)) {
			heightMap[iPos.x][iPos.y].y -= removal;
		}
		else { std::cout << "nan" << " val " << removal << std::endl; }
		if (removal < 0) deposited++;
		//std::cout << iPos.x << " " << iPos.y << " " << removal << std::endl;
		currentVolume *= (1.0 - dt * evaporationRate);
		speed *= (1.0f - dt * friction);*/
	}
	if (deposited > 0)
	std::cout << "deposited " << deposited << std::endl;
}

void HydraulicErosion::ErodeWholeMap(int iterations, float volume, float dt, float density, float friction, glm::ivec2 start) {
	float progressPerIteration = 1 / ((float)iterations);
	int range = 50.0f;
	//for (int i = 0; i < iterations; i++) {
		glm::vec2 newPos = start;
		//newPos = { randomInt(mapSize - 1), randomInt(mapSize - 1) };

		glm::vec2 speed = { 0.0f, 0.0f };
		float sediment = 0.0f;
		float currentVolume = volume;
		//std::cout << newPos.x << " " << newPos.y << std::endl;
		while (currentVolume > 0.000000001) {
			glm::ivec2 iPos = newPos;
			glm::vec3 normal = calculateNormal(iPos.x, iPos.y);
		
			speed += dt * glm::vec2(normal.x, normal.y) / (currentVolume * density);
			
			newPos += speed;
			speed *= (1.0 - dt * friction);


			if (newPos.x < 1 || newPos.x > mapSize - 2 || newPos.y < 1 || newPos.y > mapSize - 2) {
				break;
			}
			float maxSediment = currentVolume * glm::length(speed) * (heightMap[iPos.x][iPos.y].y - heightMap[(int)newPos.x][(int)newPos.y].y);
			if (maxSediment < 0.0f) maxSediment = 0.0f;
			float diff = maxSediment - sediment;
			sediment += dt * erosionRate * diff;

			float removal = dt * currentVolume * diff * erosionRate;
			if (!std::isnan(removal) && !std::isinf(removal)) {
				heightMap[iPos.x][iPos.y].y -= removal;
			}
			else { std::cout<<"nan" << " val " << removal << std::endl; }
			
			//std::cout << iPos.x << " " << iPos.y << " " << removal << std::endl;
			currentVolume *= (1.0 - dt * evaporationRate);
			//std::cout << newPos.x << " " << newPos.y << std::endl;
		}
	//}
}
