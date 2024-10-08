#include"Window.h"
#include "SunTextureGenerator.cpp"
#include <gtx/string_cast.hpp>

int Window::display(void)
{
    //terrain properties
    int customSeed = 0;
    bool useCustomSeed = false;
    float roughness = 15000.0f;
    float flatteningScale = 2.15f;
    int width = 1920;
    int height = 1080;
    const int allowedSizes[] = { 257, 513, 1025, 2049, 4097 };
    int currentMeshSizeIndex = 2;
    const char* allowedSizesLabels[] = {"257", "513", "1025", "2049", "4097"};
    bool enableTriangles = false;
    const char* generationTypeLabel[] = { "Diamond Square", "Perlin Noise" };
    const int generationType[] = { 0, 1 };
    int currentAlgorithm = 0;
    //perlin
    float baseFrequency = 0.07f;
    int octaves = 6;
    float persistance = 0.5f;
    float noiseAmplitude = 10000.0f;

    //Camera properties
    float nearplane = 10.0f;
    float farplane = 2000000.0f;
    float fov = 120.0f;
    InputMode inputMode = InputMode::CameraControl;
    bool rotateObject = false;

    //Texture properties
    float maxTextureNoise = 15000.0f;
    float minTexHeight = -100000.0f;
    float maxTexHeight = 100000.0f;
    float snowMin = 400.0f;
    float snowMax = 500.0f;
    float rockMin = 300.0f;
    float rockMax = 400.0f;
    float grassMin = 200.0f;
    float grassMax = 300.0f;
    float soilMin = 100.0f;
    float soilMax = 200.0f;
    bool useGrassTex = true;
    bool useRockTex = true;
    bool useSnowTex = true;
    bool useSoilTex = true;

    // Export
    bool isExporting = false;
    bool firstExport = false;
    bool savedToOBJ = false;
    float progress = 0.0f;
    float exportProgress = 0.0f;
    std::string exportState = "";
    std::string saveFilePath = "";
    //Skyboxes 
    const char* skyTextures[] = { "Default", "Clody sky", "Sunset", "Night"};
    int currentSkyIndex = 0;
    const char* skyboxes[] = { "Textures\\sunnySkybox.tga",
    "Textures\\grayCloudsSky.tga",
    "Textures\\yelSkybox.tga",
    "Textures\\nightSky.tga" };

    //Lighting
    bool enableShadows = true;

    unsigned int samples = 8;
    float meshPointDistance = 100.0f;
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    glfwWindowHint(GLFW_SAMPLES, samples);
    glEnable(GL_MULTISAMPLE);
    std::vector<std::vector<::glm::vec3>> map;
    int mapSize = 1025;
    //int mapSize = 25;
    map.resize(mapSize);
    for (int i = 0; i < mapSize; i++) {
        map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
    }

    std::cout << "Vector created" << std::endl;
    MapBuilder mapBuilder(map, meshPointDistance);
    PerlinNoiseGenerator noiseGen;
    Map terrain(mapSize, roughness, flatteningScale, maxTextureNoise, map, mapBuilder, noiseGen);
    std::cout << "Main Terrain gen starting" << std::endl;
    //terrain.generateWithPerlin(0.03f, 6, 0.5f, 50000000.0f);
    //terrain.searchEdgeValues();
    terrain.generate();
    //terrain.generateWithPerlin();
    minTexHeight = terrain.getMinY();
    maxTexHeight = terrain.getMaxY();
    Sphere sunSphere(500.0f, 100, 100); // SUN
    glm::vec3 sunPosition = glm::vec3(15000.0, 80000.0f, 15000.0f); // Position high above the terrain
    glm::vec3 sunScale = glm::vec3(20.0f); // Scale it to make it visible
    //sunSphere.translate(sunPosition);

    Camera camera(1920, 1080, { 0.0f, 200.0f, 0.0f }, rotateObject);
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
    glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK); // Cull back faces
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    std::cout << "Snow texture ID: " << snowTex.ID << std::endl;
    std::cout << "Grass texture ID: " << grassTex.ID << std::endl;
    std::cout << "Soil texture ID: " << soilTex.ID << std::endl;
    std::cout << "Stone texture ID: " << stoneTex.ID << std::endl;


    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
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
    glm::vec3 translation = glm::vec3(10000.0f, 10000.0f, 10000.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(10000.0f, 10000.0f, 10000.0f);
    // SKYBOX
    Shader skyboxShader("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Shaders\\skybox.vert", "C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src/Shaders\\skybox.frag");
    Skybox skybox(skyboxShader);
    skybox.initTexture("C:\\Users\\karol\\Praca_Inzynierska\\Procedural_Terrain_Generation\\Procedural_Terrain_Generation\\src\\Textures\\sunnySkybox.tga");
    std::cout << "IN WINDOW --> " << skyboxShader.ID << std::endl;
    //terrain.exportToOBJ();
    glActiveTexture(GL_TEXTURE0);
    snowTex.Bind();
    glActiveTexture(GL_TEXTURE1);
    grassTex.Bind();
    glActiveTexture(GL_TEXTURE2);
    soilTex.Bind();
    glActiveTexture(GL_TEXTURE3);
    stoneTex.Bind();
    while (!glfwWindowShouldClose(window)) {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
        //IMGUI
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        // DRAW SHADOWMAP HERE????
        shadowShader.Activate();
        shadowMap.BindForWriting();
        glUniformMatrix4fv(glGetUniformLocation(shadowShader.ID, "lighProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
        terrain.draw();
        shadowMap.Unbind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.Activate();
       /* if (!io.WantCaptureMouse) {
            camera.InputHandler(window);
        }*/
        camera.InputHandler(window, !io.WantCaptureMouse);
        // Updates and exports the camera matrix to the Vertex Shader
        camera.Matrix(shader, "camMatrix");
        camera.updateMatrix(fov, nearplane, farplane);
        //lighting 
        shader.Activate();  // Activate your terrain shader
        glUniform1f(glGetUniformLocation(shader.ID, "snowMin"), snowMin);
        glUniform1f(glGetUniformLocation(shader.ID, "snowMax"), snowMax);
        glUniform1f(glGetUniformLocation(shader.ID, "rockMin"), rockMin);
        glUniform1f(glGetUniformLocation(shader.ID, "rockMax"), rockMax);
        glUniform1f(glGetUniformLocation(shader.ID, "grassMin"), grassMin);
        glUniform1f(glGetUniformLocation(shader.ID, "grassMax"), grassMax);
        glUniform1f(glGetUniformLocation(shader.ID, "soilMin"), soilMin);
        glUniform1f(glGetUniformLocation(shader.ID, "soilMax"), soilMax);
        glUniform1f(glGetUniformLocation(shader.ID, "maxTextureNoise"), maxTextureNoise);

        glUniform1i(glGetUniformLocation(shader.ID, "useSnowTex"), useSnowTex);
        glUniform1i(glGetUniformLocation(shader.ID, "useGrassTex"), useGrassTex);
        glUniform1i(glGetUniformLocation(shader.ID, "useRockTex"), useRockTex);
        glUniform1i(glGetUniformLocation(shader.ID, "useSoilTex"), useSoilTex);
        shader.setVec3("sunDirection", sunDirection);
        shader.setVec3("lightColor", lightColor);  // Warm white light
        shader.setVec3("viewPos", camera.position);
        terrain.uploadNoiseTexture(shader);
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
        /*glActiveTexture(GL_TEXTURE0);
        snowTex.Bind();
        glActiveTexture(GL_TEXTURE1);
        grassTex.Bind();
        glActiveTexture(GL_TEXTURE2);
        soilTex.Bind();
        glActiveTexture(GL_TEXTURE3);
        stoneTex.Bind();*/
        if (enableShadows) {
            shadowMap.BindForReading(GL_TEXTURE6);
            glActiveTexture(GL_TEXTURE6);
        }
        terrain.draw();
       /* snowTex.Unbind();
        stoneTex.Unbind();
        grassTex.Unbind();
        soilTex.Unbind();*/
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
        //sunSphere.Draw(sunShader, sunPosition, sunScale, glm::vec3(1.0f, 1.0f, 0.0f)); // Drawing the sphere as the sun
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
        if (enableTriangles) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (useCustomSeed) {
            srand(customSeed);
        }
        else {
            srand(time(NULL));
        }
        

        ImGui::Begin("Terrain properties");
        ImGui::Text("Algorithm");
        ImGui::SameLine();
        if (ImGui::Combo("##algorithmSelection", &currentAlgorithm, generationTypeLabel, IM_ARRAYSIZE(generationType))) {
            currentAlgorithm = generationType[currentAlgorithm];
        }
        ImGui::Text("Use custom seed");
        ImGui::SameLine();
        ImGui::Checkbox("##seedChechbox", &useCustomSeed);
        ImGui::Text("Custom seed");
        ImGui::SameLine();
        ImGui::InputInt("##customSeed", &customSeed);
        //only in diamond square
        if (currentAlgorithm == 0) {
            ImGui::Text("Elevation");
            ImGui::SameLine();
            ImGui::InputFloat("##elevation", &roughness, 0.0f, 0.0f, "%.2f");
            ImGui::Text("Flattening scale");
            ImGui::SameLine();
            ImGui::InputFloat("##flatteningScale", &flatteningScale, 0.0f, 0.0f, "%.2f");
        }
        else if (currentAlgorithm == 1) {
            ImGui::Text("Base frequency");
            ImGui::SameLine();
            ImGui::SliderFloat("##baseFreqSlider", &baseFrequency, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Octaves");
            ImGui::SameLine();
            ImGui::SliderInt("##octavesSlider", &octaves, 1.0f, 10.0f);
            ImGui::Text("Persistance");
            ImGui::SameLine();
            ImGui::SliderFloat("##persistanceSlider", &persistance, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Amplitude");
            ImGui::SameLine();
            ImGui::InputFloat("##amplitudeInput", &noiseAmplitude, 0.0f, 0.0f, "%.2f");
        }
        ImGui::Text("Mesh size");
        ImGui::SameLine();
        if (ImGui::Combo("##meshSizeSelection", &currentMeshSizeIndex, allowedSizesLabels, IM_ARRAYSIZE(allowedSizesLabels))) {
            mapSize = allowedSizes[currentMeshSizeIndex];
        }
        ImGui::Text("Mesh offset");
        ImGui::SameLine();
        ImGui::InputFloat("#Distance between mesh points", &meshPointDistance, 0.0f, 0.0f, "%.2f");
        ImGui::Text("Show mesh");
        ImGui::SameLine();
        ImGui::Checkbox("##showmeshCheckbox", &enableTriangles);
        if (ImGui::Button("Generate")) {
            terrain.cleanUpObjects();
            bool genFinished = false;
            ImGui::Text("Generating terrain...");
            map.resize(mapSize);
            for (int i = 0; i < mapSize; i++) {
                map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
            }
            mapBuilder.setOffset(meshPointDistance);
            if (currentAlgorithm == 0) {
                terrain.setRoughness(roughness);
                terrain.setFlattening(flatteningScale);
                terrain.setSize(mapSize);
                terrain.generate();
            }
            else if (currentAlgorithm == 1) {
                terrain.generateWithPerlin(baseFrequency, octaves, persistance, noiseAmplitude);
            }
            terrain.searchEdgeValues();
            terrain.initObjects();
            minTexHeight = terrain.getMinY();
            maxTexHeight = terrain.getMaxY();
        }

        ImGui::End();

        ImGui::Begin("Camera");
        ImGui::Text("Camera fov");
        ImGui::SameLine();
        ImGui::SliderFloat("##fovSlider", &fov, 60.f, 180.0f, "%.2f");
        ImGui::Text("Nearplane");
        ImGui::SameLine();
        ImGui::InputFloat("##inputNearplane", &nearplane);
        //ImGui::SliderFloat("##nearplane", &nearplane, 0.1f, 200000000.0f, "%.2f");
        ImGui::Text("Farplane");
        ImGui::SameLine();
        ImGui::InputFloat("##inputFarplane", &farplane);
        ImGui::Text("Rotate map");
        ImGui::SameLine();
        ImGui::Checkbox("##rotateMap", &rotateObject);
        //ImGui::SliderFloat("##farplane", &farplane, 1.0f, 200000000.0f, "%.2f");

        ImGui::End();

        ImGui::Begin("Textures");
        ImGui::Text("Skybox:");
        ImGui::SameLine();
        if (ImGui::Combo("##SkyboxChoice", &currentSkyIndex, skyTextures, IM_ARRAYSIZE(skyTextures))) {
            //mapSize = allowedSizes[currentMeshSizeIndex];
            skybox.initTexture(skyboxes[currentSkyIndex]);
        }
        //Soil
        ImGui::Text("Soil texture");
        ImGui::SameLine();
        ImGui::Checkbox("Use soil texture", &useSoilTex);
        ImGui::Text("Min height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##soilMinSlider", &soilMin, minTexHeight, maxTexHeight, "%.2f");
        ImGui::Text("Max height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##soilMaxSlider", &soilMax, minTexHeight, maxTexHeight, "%.2f");
        // Grass
        ImGui::Text("Grass texture");
        ImGui::SameLine();
        ImGui::Checkbox("Use grass texture", &useGrassTex);
        ImGui::Text("Min height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##grassMinSlider", &grassMin, minTexHeight, maxTexHeight, "%.2f");
        ImGui::Text("Max height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##grassMaxSlider", &grassMax, minTexHeight, maxTexHeight, "%.2f");

        // Rock
        ImGui::Text("Rock texture");
        ImGui::SameLine();
        ImGui::Checkbox("Use rock texture", &useRockTex);
        ImGui::Text("Min height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##rockMinSlider", &rockMin, minTexHeight, maxTexHeight, "%.2f");
        ImGui::Text("Max height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##rockMaxSlider", &rockMax, minTexHeight, maxTexHeight, "%.2f");

        // Snow
        ImGui::Text("Snow texture");
        ImGui::SameLine();
        ImGui::Checkbox("Use snow texture", &useSnowTex);
        ImGui::Text("Min height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##snowMinSlider", &snowMin, minTexHeight, maxTexHeight, "%.2f");
        ImGui::Text("Max height treshold");
        ImGui::SameLine();
        ImGui::SliderFloat("##snowMaxSlider", &snowMax, minTexHeight, maxTexHeight, "%.2f");
        ImGui::End();

        ImGui::Begin("Lighting");
        ImGui::Text("Enable shadows");
        ImGui::SameLine();
        ImGui::Checkbox("##shadowsCheckbox", &enableShadows);
        ImGui::Text("Directional light");
        ImGui::Text("Direction");
        ImGui::SameLine();
        ImGui::DragFloat3("##lightDirection", glm::value_ptr(sunDirection), 0.01f, -1.0f, 1.0f, "%.2f");
        sunDirection = glm::normalize(sunDirection);  // Normalize direction
        ImGui::Text("Light Color");
        ImGui::ColorEdit3("##lightColor", glm::value_ptr(lightColor));
        ImGui::End();
        // Exportinh
        ImGui::Begin("Export terrain");
        if (ImGui::Button("Export to OBJ")) {
            if (!isExporting) {
                saveFilePath = terrain.showSaveFileDialog("terrain.obj");
                if (!saveFilePath.empty()) {
                    isExporting = true;
                    std::thread exportThread([&, saveFilePath]() {
                        terrain.exportToOBJ(saveFilePath);
                        isExporting = false;
                        savedToOBJ = true;
                    });
                    exportThread.detach();
                }
            }
            else {
                ImGui::SameLine();
                ImGui::Text("Export already in progress");
            }
        }
        if (isExporting) {
            exportProgress = terrain.getExportingProgress();
            exportState = terrain.getExportState();
            if (exportProgress > progress) {
                progress += 0.0003f;
            }
            if (progress >= 1.0f) {
                progress = 1.0f;
            }
            ImGui::Text(exportState.c_str());
            ImGui::ProgressBar(progress, ImVec2(0.0f, 0.0f));
        }
        else {
            progress = 0.0f;
        }
        if (savedToOBJ) {
            ImGui::Text(("Saved to " + saveFilePath).c_str());
            if (ImGui::Button("Dismiss")) {
                savedToOBJ = false;
            }

        }
        ImGui::End();
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}