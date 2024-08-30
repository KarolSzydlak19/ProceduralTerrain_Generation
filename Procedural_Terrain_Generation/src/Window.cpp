#include"Window.h"
#include "SunTextureGenerator.cpp"

int Window::display(void)
{
    float roughness = 200.0f;
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
    int mapSize = 1025;
    //int mapSize = 25;
    map.resize(mapSize);
    for (int i = 0; i < mapSize; i++) {
        map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
    }

    std::cout << "Vector created" << std::endl;
    MapBuilder mapBuilder(map);
    Map terrain(mapSize, roughness, map, mapBuilder);
    std::cout << "Main Terrain gen starting" << std::endl;
    terrain.generate();
    Sphere sunSphere(50.0f, 100, 100); // SUN
    glm::vec3 sunPosition = glm::vec3(20.0, 4000.0f, 0.0f); // Position high above the terrain
    glm::vec3 sunScale = glm::vec3(20.0f); // Scale it to make it visible
    //sunSphere.translate(sunPosition);

    Camera camera(1920, 1080, { 0.0f, 20.0f, 0.0f });
    Shader sunShader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\sun.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\sun.frag");
    Shader shader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\default.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\default.frag");
    Texture mapTexture("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\rockyTex.tga", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    mapTexture.texUnit(shader, "mapSurfaceTexture", 0);
    Texture sunTex("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\sunTex.tga", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
    sunTex.texUnit(sunShader, "sunTexture", 0);
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
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK); // Cull back faces
    //glFrontFace(GL_CCW);
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        shader.Activate();
        camera.InputHandler(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(shader, "camMatrix");
        camera.updateMatrix(120.0f, 0.1f, 10000.0f);
        mapTexture.Bind();
        terrain.draw();
        mapTexture.Unbind();       
        sunShader.Activate();
        // Now draw the sphere
        camera.Matrix(sunShader, "sunCamMatrix");
        if (glGetError() != GL_NO_ERROR) {
            std::cerr << "OpenGL Error after cam matrix" << std::endl;
        }
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, sunPosition);
        model = glm::scale(model, sunScale);
        //sunShader.setMat4("model", model);
        GLint modelLoc = glGetUniformLocation(sunShader.ID, "model");
        if (modelLoc == -1) {
            std::cerr << "Uniform 'model' not found or not used in the shader!" << std::endl;
        }
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        sunTex.Bind();
        sunSphere.Draw(sunShader, sunPosition, sunScale, glm::vec3(1.0f, 1.0f, 0.0f)); // Drawing the sphere as the sun
        sunTex.Unbind();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}