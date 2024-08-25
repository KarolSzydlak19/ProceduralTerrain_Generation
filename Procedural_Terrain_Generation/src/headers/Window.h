#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb/stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <thread>
#include <functional>
#include <vector>
#include "Map.h"
#include "Sphere.h"
#include <Camera.h>
#include <filesystem>
#ifndef WINDOW_H
#define WINDOW_H

class Window {
public:
	int display(void);
	Window() {};
};

#endif 