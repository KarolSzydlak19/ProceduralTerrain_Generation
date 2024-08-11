#include"Window.h"

int Window::display(void)
{
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
    std::vector<std::vector<::glm::vec3>> map;
    int mapSize = 4097;
    //int mapSize = 25;
    map.resize(mapSize);
    for (int i = 0; i < mapSize; i++) {
        map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
    }

    std::cout << "Vector created" << std::endl;
    MapBuilder mapBuilder(map);
    Map terrain(mapSize, 800.0f, map, mapBuilder);
    std::cout << "Main Terrain gen starting" << std::endl;
    terrain.generate();
    

    Camera camera(1920, 1080, { 0.0f, 20.0f, 0.0f });
    Shader shader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\default.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\default.frag");
    Texture mapTexture("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\rockyTex.tga", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    mapTexture.texUnit(shader, "mapSurfaceTexture", 0);
    if (std::filesystem::exists("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\rockyTex.tga")) {
        std::cout << "File good" << std::endl;
    }
    else {
        std::cout << "File no good" << std::endl;
    }

    /* Loop until the user closes the window */
    std::cout << "Infinite loop " << std::endl;
    terrain.initObjects();
    //terrain.print();
    glEnable(GL_DEPTH_TEST); // Enable depth testing
    glClearColor(0.0f, 0.45f, 0.63f, 1.0f);
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        shader.Activate();
        camera.InputHandler(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(120.0f, 0.1f, 10000.0f, shader, "camMatrix");
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