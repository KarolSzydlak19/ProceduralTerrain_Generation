#pragma once
#include<glm/glm.hpp>

class Drop {
public:
	glm::vec2 pos;
	float speed;
	float sediment;
	float density;
	float maxSediment;
	Drop(glm::vec2 pos, float density, float maxSediment) : pos(pos), density(density), maxSediment(maxSediment) {
		speed = 0.0f;
		sediment = 0.0f;
	}
};