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
#include <Camera.h>
#include <filesystem>
namespace fs = std::filesystem;
int main() {
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(1920, 1080, "PTG", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    std::cout << "Current working directory: " << fs::current_path() << std::endl;
    std::cout << "starting" << std::endl;
	std::vector<std::vector<::glm::vec3>> map;
    std::cout << "Vector" << std::endl;
	int mapSize = 4097;
	map.resize(mapSize);
	for (int i = 0; i < mapSize; i++) {
		map[i].resize(mapSize, {0.0f, 0.0f, 0.0f});
	}
    std::cout << "Vector created" << std::endl;
	Map terrain(mapSize, 50.0f, map);
	//terrain.print();
    std::cout << "Main Terrain gen starting" << std::endl;
	terrain.generate();
	//terrain.print();
    std::cout << "Terrain generated" << std::endl;
    std::cout << "Creating camera" << std::endl;
    Camera camera(1920, 1080, { 0.0f, 20.0f, 0.0f });
    std::cout << "Creating shader" << std::endl;
    Shader shader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\default.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\default.frag");
    Texture mapTexture("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\rockyTex.tga", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    mapTexture.texUnit(shader, "mapSurfaceTexture", 0);
    if (fs::exists("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\rockyTex.tga")) {
        std::cout << "File good" << std::endl;
    }
    else {
        std::cout << "File no good" << std::endl;
    }
        /* Loop until the user closes the window */
    std::cout << "Infinite loop "<< std::endl;
    terrain.initObjects();
    //terrain.print();
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        shader.Activate();
        camera.InputHandler(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(90.0f, 0.1f, 1000.0f, shader, "camMatrix");
        mapTexture.Bind();
        terrain.draw();
        mapTexture.Unbind();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
	return 0;
}