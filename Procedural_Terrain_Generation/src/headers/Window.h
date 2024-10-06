#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
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
#include <thread>
#include "Map.h"
#include "Sphere.h"
#include "ShadowMap.h"
#include "Skybox.h"
#include "InputMode.cpp"
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