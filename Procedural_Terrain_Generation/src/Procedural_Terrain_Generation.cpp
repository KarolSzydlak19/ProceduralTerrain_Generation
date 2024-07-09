#include<iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>
#include<thread>
#include<functional>
#include<vector>
#include "Map.h"

int main() {

	std::vector<std::vector<::glm::vec3>> map;
	map.resize(5);
	for (int i = 0; i < 5; i++) {
		map[i].resize(5);
	}
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			map[i].push_back({ 0.0f,0.0f,0.0f });
		}
	}
	Map terrain(5, 100.0f, map);
	//terrain.print();
	terrain.generate();
	terrain.print();
	return 0;
}