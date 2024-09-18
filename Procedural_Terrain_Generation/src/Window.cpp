#include"Window.h"
#include "SunTextureGenerator.cpp"
#include <gtx/string_cast.hpp>

int Window::display(void)
{
    float roughness = 15000.0f;
    int width = 1920;
    int height = 1080;
    float farplane = 2000000.0f;
    GLFWwindow* window;
    /* Initialize the library */
    if (!glfwInit()) {
        return -1;
    }

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(width, height, "PTG", NULL, NULL);
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
    Sphere sunSphere(500.0f, 100, 100); // SUN
    glm::vec3 sunPosition = glm::vec3(15000.0, 80000.0f, 15000.0f); // Position high above the terrain
    glm::vec3 sunScale = glm::vec3(20.0f); // Scale it to make it visible
    //sunSphere.translate(sunPosition);

    Camera camera(1920, 1080, { 0.0f, 20.0f, 0.0f });
    Shader sunShader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\sun.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\sun.frag");
    Shader shader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\default.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\default.frag");
   // Texture mapTexture("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\rockyTex.tga", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_UNSIGNED_BYTE);
   

    Texture snowTex("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\snow.tga", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    Texture grassTex("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\grass.tga", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
    Texture soilTex("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\soil.tga", GL_TEXTURE_2D, GL_TEXTURE2, GL_RGB, GL_UNSIGNED_BYTE);
    Texture stoneTex("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\stone2.tga", GL_TEXTURE_2D, GL_TEXTURE3, GL_RGB, GL_UNSIGNED_BYTE);
    //mapTexture.texUnit(shader, "mapSurfaceTexture", 0);
    std::cout << "tex loaded" << std::endl;
    snowTex.texUnit(shader, "snowTexture", 0);
    grassTex.texUnit(shader, "grassTexture", 1);
    soilTex.texUnit(shader, "soilTexture", 2);
    stoneTex.texUnit(shader, "stoneTexture", 3);
    Texture sunTex("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\sunTex.tga", GL_TEXTURE_2D, GL_TEXTURE4, GL_RGBA, GL_UNSIGNED_BYTE);
    sunTex.texUnit(sunShader, "sunTexture", 4);
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
    //glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); // Cull back faces
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    std::cout << "Snow texture ID: " << snowTex.ID << std::endl;
    std::cout << "Grass texture ID: " << grassTex.ID << std::endl;
    std::cout << "Soil texture ID: " << soilTex.ID << std::endl;
    std::cout << "Stone texture ID: " << stoneTex.ID << std::endl;


    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 0.80f);
    glm::vec3 sunDirection = glm::normalize(glm::vec3(0.2f, 0.6f, 0.1f));  // Direction vector for sunlight
    // SHADOWS
    ShadowMap shadowMap(2048, 2048);
    glm::mat4 orthagonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
    glm::mat4 lightView = glm::lookAt(20.0f * sunDirection, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = orthagonalProjection * lightView;
    Shader shadowShader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\shadowMap.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\shadowMap.frag");
    shadowShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shadowShader.ID, "lighProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
    Shader shadowDis("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\shadowMap.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\shadowMap.frag");
    //glFrontFace(GL_CCW);
    glm::mat4 matrix = glm::mat4(1.0f);
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    // SKYBOX
    Shader skyboxShader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\skybox.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\skybox.frag");
    Skybox skybox(skyboxShader);
    skybox.initTexture("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\StandardCubeMap.tga");
    std::cout << "IN WINDOW --> " << skyboxShader.ID << std::endl;
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        // DRAW SHADOWMAP HERE????
        shadowShader.Activate();
        shadowMap.BindForWriting();
        glUniformMatrix4fv(glGetUniformLocation(shadowShader.ID, "lighProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        terrain.draw();
        shadowMap.Unbind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Activate();
        camera.InputHandler(window);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(shader, "camMatrix");
        camera.updateMatrix(120.0f, 0.1f, farplane);
        //lighting 
        shader.Activate();  // Activate your terrain shader
        shader.setVec3("sunDirection", sunDirection);
        shader.setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));  // Warm white light
        shader.setVec3("viewPos", camera.position);
        // Transform the matrices to their correct form
        glm::mat4 trans = glm::translate(trans, translation);
        glm::mat4 rot = glm::mat4_cast(rotation);
        glm::mat4 sca = glm::scale(sca, scale);

        // Push the matrices to the vertex shader (POPRAWIC NA UZYCIE SETMAT4)
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
        glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));
        //mapTexture.Bind();
        glActiveTexture(GL_TEXTURE0);
        snowTex.Bind();
        glActiveTexture(GL_TEXTURE1);
        grassTex.Bind();
        glActiveTexture(GL_TEXTURE2);
        soilTex.Bind();
        glActiveTexture(GL_TEXTURE3);
        stoneTex.Bind();
        shadowMap.BindForReading(GL_TEXTURE6);
        glActiveTexture(GL_TEXTURE6);
        terrain.draw();
        snowTex.Unbind();
        stoneTex.Unbind();
        grassTex.Unbind();
        soilTex.Unbind();
        //mapTexture.Unbind();       
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
        glActiveTexture(GL_TEXTURE4);
        sunTex.Bind();
        sunSphere.Draw(sunShader, sunPosition, sunScale, glm::vec3(1.0f, 1.0f, 0.0f)); // Drawing the sphere as the sun
        sunTex.Unbind();
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::mat4(glm::mat3(glm::lookAt(camera.position, camera.position + camera.orientation, camera.up)));
        projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, farplane);
        skybox.draw(view, projection);
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            std::cerr << "OpenGL Error: " << error << std::endl;
        }
        //terrain.drawNormals();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}