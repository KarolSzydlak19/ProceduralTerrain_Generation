#include"Window.h"
#include "SunTextureGenerator.cpp"
#include <gtx/string_cast.hpp>

int Window::display(void)
{
    const float TARGET_FPS = 60.0f;
    const float FRAME_DURATION = 1.0f / TARGET_FPS;
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
    float terrainScale = 1.0f;
    //perlin
    float baseFrequency = 0.07f;
    int octaves = 6;
    float persistance = 0.5f;
    float noiseAmplitude = 10000.0f;

    //Hydraulic erosion
    float waterAdditionRate = 0.01f;
    float sedimentCapacity = 250.0f;//0.00001f;
    float evaporationRate = 0.015f;
    float depositionRate = 0.05f; // 0.000001f;
    float erosionRate = 0.04f;//0.0000001f;
    int hydraulicErosionIterations = 100;
    bool isEroding = false;
    float erosionProgress = 0.0f;
    bool justErodet = false;
    float volume = 50.0f;
    float dt = 1.0f;
    float density = 0.1f;
    float friction = 0.04f;

    //Camera properties
    float nearplane = 10.0f;
    float farplane = 2000000.0f;
    float fov = 120.0f;
    InputMode inputMode = InputMode::CameraControl;
    bool rotateObject = false;
    float cameraSpeed = 500.0f;

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
    bool savedToFile = false;
    float progress = 0.0f;
    float exportProgress = 0.0f;
    std::string exportState = "";
    std::string saveFilePath = "";
    //Skyboxes 
    const char* skyTextures[] = { "Default", "Clody sky", "Sunset", "Night"};
    int currentSkyIndex = 0;
    const char* skyboxes[] = {TEX_DIR "/sunnySkybox.tga",
    TEX_DIR "/grayCloudsSky.tga",
    TEX_DIR "/yelSkybox.tga",
    TEX_DIR "/nightSky.tga" };

    //Lighting
    bool enableShadows = true;
    float ambient = 0.15f;
    float ambientMaterial = 1.0f;
    float vDiffuseMaterial = 0.96f;
    float vDiffuseLight = 0.86f;
    float vSpecularMaterial = 0.15f;
    float vSpecularLight = 0.97f;
    float shininess = 16.0f;

    //Testing
    int numberOfTests = 10;
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

    // Shader
    Shader shader(SHADER_DIR "/default.vert", SHADER_DIR "/default.frag");
    Shader shadowShader(SHADER_DIR "/shadowMap.vert", SHADER_DIR "/shadowMap.frag");
    Shader shadowDis(SHADER_DIR "/shadowMap.vert", SHADER_DIR"/shadowMap.frag");
    Shader skyboxShader(SHADER_DIR "/skybox.vert", SHADER_DIR "/skybox.frag");

    //Textures 
    Texture snowTex(TEX_DIR "/snow.tga", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGB, GL_UNSIGNED_BYTE);
    Texture grassTex(TEX_DIR "/grass.tga", GL_TEXTURE_2D, GL_TEXTURE1, GL_RGB, GL_UNSIGNED_BYTE);
    Texture soilTex(TEX_DIR "/soil.tga", GL_TEXTURE_2D, GL_TEXTURE2, GL_RGB, GL_UNSIGNED_BYTE);
    Texture stoneTex(TEX_DIR "/stone2.tga", GL_TEXTURE_2D, GL_TEXTURE3, GL_RGB, GL_UNSIGNED_BYTE);

    snowTex.texUnit(shader, "snowTexture", 0);
    grassTex.texUnit(shader, "grassTexture", 1);
    soilTex.texUnit(shader, "soilTexture", 2);
    stoneTex.texUnit(shader, "stoneTexture", 3);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
    ImGui::GetIO().IniFilename = P_DIR;

    glfwWindowHint(GLFW_SAMPLES, samples);
    glEnable(GL_MULTISAMPLE);
    std::vector<std::vector<::glm::vec3>> map;
    int mapSize = 1025;
    map.resize(mapSize);
    for (int i = 0; i < mapSize; i++) {
        map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
    }

    MapBuilder mapBuilder(map, meshPointDistance);
    PerlinNoiseGenerator noiseGen;
    Map terrain(mapSize, roughness, flatteningScale, maxTextureNoise, map, mapBuilder, noiseGen);
    GenerationTimer genTimer(terrain);
    HydraulicErosion hydraulicEroder(map);

    terrain.generate();
    minTexHeight = terrain.getMinY();
    maxTexHeight = terrain.getMaxY();

    Camera camera(1920, 1080, { 0.0f, 200.0f, 0.0f }, rotateObject);

    // Loop until the user closes the window 
    terrain.initObjects();
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.0f, 0.45f, 0.63f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::cout << "Snow texture ID: " << snowTex.ID << std::endl;
    std::cout << "Grass texture ID: " << grassTex.ID << std::endl;
    std::cout << "Soil texture ID: " << soilTex.ID << std::endl;
    std::cout << "Stone texture ID: " << stoneTex.ID << std::endl;
    glm::vec3 lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 sunDirection = glm::normalize(glm::vec3(0.80f, 0.16f, -0.58f));  // Direction vector for sunlight

    // SHADOWS
    ShadowMap shadowMap(2048, 2048);
    glm::mat4 orthagonalProjection = glm::ortho(-35.0f, 35.0f, -35.0f, 35.0f, 0.1f, 75.0f);
    glm::mat4 lightView = glm::lookAt(20.0f * sunDirection, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightProjection = orthagonalProjection * lightView;
    shadowShader.Activate();
    glUniformMatrix4fv(glGetUniformLocation(shadowShader.ID, "lighProjection"), 1, GL_FALSE, glm::value_ptr(lightProjection));
    glm::mat4 matrix = glm::mat4(1.0f);
    glm::vec3 translation = glm::vec3(10000.0f, 10000.0f, 10000.0f);
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(10000.0f, 10000.0f, 10000.0f);

    /*glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 rot = glm::mat4_cast(rotation);  
    glm::vec3 terrainScaleVec = glm::vec3(terrainScale, terrainScale * 0.5f, terrainScale);
    glm::mat4 sca = glm::scale(glm::mat4(1.0f), terrainScaleVec);
    glm::mat4 model = trans * rot * sca;*/

    // SKYBOX
    Skybox skybox(skyboxShader);
    skybox.initTexture(TEX_DIR "/sunnySkybox.tga");
    shader.Activate();
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

    //FPS
    double previousTime = glfwGetTime();
    double currentTime;
    double deltaTime;

    while (!glfwWindowShouldClose(window)) {
        currentTime = glfwGetTime();
        deltaTime = currentTime - previousTime;
        if (deltaTime >= FRAME_DURATION) {
            previousTime = currentTime;
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
            camera.InputHandler(window, !io.WantCaptureMouse);
            // Updates and exports the camera matrix to the Vertex Shader
            camera.Matrix(shader, "camMatrix");
            camera.updateMatrix(fov, nearplane, farplane);
            camera.speed = cameraSpeed;
            //lighting
            glm::mat4 model = glm::mat4(1.0f);

            // 1. Apply scaling first to avoid shifting
            glm::vec3 terrainScaleVec = glm::vec3(terrainScale, terrainScale * 0.5f, terrainScale);
            model = glm::scale(model, terrainScaleVec);

            // 2. Apply rotation (if needed)
            model = model * glm::mat4_cast(rotation);

            // 3. Apply translation last to move the terrain to its correct position
            model = glm::translate(model, translation);

            /*terrainScaleVec = glm::vec3(terrainScale, terrainScale * 0.5f, terrainScale);
            sca = glm::scale(glm::mat4(1.0f), terrainScaleVec);
            model = trans * rot * sca;*/
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

            //light parameters
            glUniform1f(glGetUniformLocation(shader.ID, "ambient"), ambient);
            glUniform1f(glGetUniformLocation(shader.ID, "vDiffuseLight"), vDiffuseLight);
            glUniform1f(glGetUniformLocation(shader.ID, "vSpecularLight"), vSpecularLight);
            glUniform1f(glGetUniformLocation(shader.ID, "vDiffuseMaterial"), vDiffuseMaterial);
            glUniform1f(glGetUniformLocation(shader.ID, "vSpecularMaterial"), vSpecularMaterial);
            glUniform1f(glGetUniformLocation(shader.ID, "shininess"), shininess);
            glUniform1f(glGetUniformLocation(shader.ID, "ambientMaterial"), ambientMaterial);

            shader.setVec3("sunDirection", sunDirection);
            shader.setVec3("lightColor", lightColor);  // Warm white light
            shader.setVec3("viewPos", camera.position);

            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(model));

            terrain.draw();

            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);
            view = glm::mat4(glm::mat3(glm::lookAt(camera.position, camera.position + camera.orientation, camera.up)));
            projection = glm::perspective(glm::radians(45.0f), (float)width / height, 0.1f, farplane);
            skybox.draw(view, projection);

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
                std::cout << "start" << std::endl;
                map.resize(mapSize);
                for (int i = 0; i < mapSize; i++) {
                    map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
                }
                std::cout << "after resize" << std::endl;
                terrain.setSize(mapSize);
                mapBuilder.setOffset(meshPointDistance);
                if (currentAlgorithm == 0) {
                    terrain.setRoughness(roughness);
                    terrain.setFlattening(flatteningScale);
                    terrain.generate();
                }
                else if (currentAlgorithm == 1) {
                    std::cout << "here" << std::endl;
                    terrain.generateWithPerlin(baseFrequency, octaves, persistance, noiseAmplitude);
                }
                terrain.searchEdgeValues();
                terrain.initObjects();
                minTexHeight = terrain.getMinY();
                maxTexHeight = terrain.getMaxY();
            }
            ImGui::Text("Scale");
            ImGui::SameLine();
            ImGui::SliderFloat("##scaleSlider", &terrainScale, 1.0f, 100.0f, "%.2f");

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
            ImGui::Text("Camera speed");
            ImGui::SameLine();
            ImGui::SliderFloat("##cameraSpeedSlider", &cameraSpeed, 0.1f, 1000.0f, " %.2f");
            //ImGui::SliderFloat("##farplane", &farplane, 1.0f, 200000000.0f, "%.2f");
            ImGui::End();
            ImGui::Begin("Hydraulic erosion");
           // ImGui::Text("Water addition rate");
            //ImGui::SameLine();
            //ImGui::InputFloat("##waterAdd", &waterAdditionRate, 0.0f, 0.0f, "%.6f");
            ImGui::Text("Sediment capacity");
            ImGui::SameLine();
            ImGui::InputFloat("##sedCap", &sedimentCapacity, 0.0f, 0.0f, "%.15f");
            //ImGui::Text("Evaporation rate");
            //ImGui::SameLine();
           // ImGui::InputFloat("##evapRate", &evaporationRate, 0.0f, 0.0f, "%.6f");
            ImGui::Text("Deposition rate");
            ImGui::SameLine();
            ImGui::InputFloat("##deposRate", &depositionRate, 0.0f, 0.0f, "%.15f");
            ImGui::Text("Erosion rate");
            ImGui::SameLine();
            ImGui::InputFloat("##erosionRate", &erosionRate, 0.0f, 0.0f, "%.15f");
            ImGui::Text("Volume");
            ImGui::SameLine();
            ImGui::InputFloat("##VolumeLevel", &volume, 0.0f, 0.0f, "%.6f");
            ImGui::Text("dt");
            ImGui::SameLine();
            ImGui::InputFloat("##dtIn", &dt, 0.0f, 0.0f, "%.6f");
            ImGui::Text("Density");
            ImGui::SameLine();
            ImGui::InputFloat("##densityIn", &density, 0.0f, 0.0f, "%.6f");
            ImGui::Text("Friction");
            ImGui::SameLine();
            ImGui::InputFloat("##frictionIn", &friction, 0.0f, 0.0f, "%.6f");
            ImGui::Text("Evaporation rate");
            ImGui::SameLine();
            ImGui::InputFloat("##evapRate", &evaporationRate, 0.0f, 0.0f, "%.6f");
            ImGui::Text("Iterations");
            ImGui::SameLine();
            ImGui::InputInt("##hyderosioniter", &hydraulicErosionIterations);
            if (ImGui::Button("Perform hydraulic errosion")) {
                hydraulicEroder.setWaterAdditionRate(waterAdditionRate);
                hydraulicEroder.setSedimentCapacity(sedimentCapacity);
                hydraulicEroder.setEvaporationRate(evaporationRate);
                hydraulicEroder.setDepositionRate(depositionRate);
                hydraulicEroder.setDepositionRate(depositionRate);
                hydraulicEroder.setErosionRate(erosionRate);
                hydraulicEroder.setMapSize(map.size());
                //hydraulicEroder.performErosion(hydraulicErosionIterations);
                //hydraulicEroder.Erode(hydraulicErosionIterations, volume, dt, density, friction);
                for (int q = 0; q < hydraulicErosionIterations; q++) {
                    std::cout << q << std::endl;
                    //hydraulicEroder.ErodeWholeMap(hydraulicErosionIterations, volume, dt, density, friction, glm::ivec2{ 110,110 });
                    for (int i = 0; i < mapSize - 1; i++) {
                        for (int j = 0; j < mapSize - 1; j++) {
                            //hydraulicEroder.ErodeWholeMap(hydraulicErosionIterations, volume, dt, density, friction, glm::ivec2{ i,j });
                            hydraulicEroder.Erode(q, volume, dt, density, friction, glm::ivec2{ i,j });
                       }
                    }
                    //hydraulicEroder.Erode(q, volume, dt, density, friction, glm::ivec2{ 0,0 });
                }
                terrain.cleanUpObjects();
                terrain.initAxes();
                terrain.initObjects();
            }
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
            /*ImGui::Text("Enable shadows");
            ImGui::SameLine();
            ImGui::Checkbox("##shadowsCheckbox", &enableShadows);*/
            ImGui::Text("Directional light");
            ImGui::Text("Direction        ");
            ImGui::SameLine();
            ImGui::DragFloat3("##lightDirection", glm::value_ptr(sunDirection), 0.01f, -1.0f, 1.0f, "%.2f");
            sunDirection = glm::normalize(sunDirection);  // Normalize direction
            ImGui::Text("Light Color      ");
            ImGui::SameLine();
            ImGui::ColorEdit3("##lightColor", glm::value_ptr(lightColor));
            ImGui::Text("Ambient light    ");
            ImGui::SameLine();
            //ImGui::InputFloat("##ambientInput", &ambient, 0.0f, 0.0f, "%.2f");
            ImGui::SliderFloat("##amb", &ambient, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Diffuse light    ");
            ImGui::SameLine();
            //ImGui::InputFloat("##DiffuseLight", &vDiffuseLight, 0.0f, 0.0f, "%.2f");
            ImGui::SliderFloat("##diff", &vDiffuseLight, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Specular light   ");
            ImGui::SameLine();
            //ImGui::InputFloat("##specLightInput", &vSpecularLight, 0.0f, 0.0f, "%.2f");
            ImGui::SliderFloat("##spec", &vSpecularLight, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Ambient material ");
            ImGui::SameLine();
            ImGui::SliderFloat("##ambientMat", &ambientMaterial, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Material diffuse ");
            ImGui::SameLine();
            //ImGui::InputFloat("##DiffuseMav", &vDiffuseMaterial, 0.0f, 0.0f, "%.2f");
            ImGui::SliderFloat("##diffm", &vDiffuseMaterial, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Material specular");
            ImGui::SameLine();
            //ImGui::InputFloat("##specMatInput", &vSpecularMaterial, 0.0f, 0.0f, "%.2f");
            ImGui::SliderFloat("##specm", &vSpecularMaterial, 0.0f, 1.0f, "%.2f");
            ImGui::Text("Shininess        ");
            ImGui::SameLine();
            ImGui::InputFloat("##shininessIn", &shininess, 0.0f, 0.0f, "%.2f");
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
                            savedToFile = true;
                        });
                        exportThread.detach();
                    }
                }
                else {
                    ImGui::SameLine();
                    ImGui::Text("Export already in progress");
                }
            }
            if (ImGui::Button("Export heightmap")) {
                if (!isExporting) {
                    saveFilePath = terrain.showSaveFileDialog("terrain_heightmap.png");
                    if (!saveFilePath.empty()) {
                        isExporting = true;
                        std::thread exportThread([&, saveFilePath]() {
                            terrain.exportToHeightmap(map, saveFilePath);
                            isExporting = false;
                            savedToFile = true;
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
            if (savedToFile) {
                ImGui::Text(("Saved to " + saveFilePath).c_str());
                if (ImGui::Button("Dismiss")) {
                    savedToFile = false;
                }

            }
            ImGui::End();

           /* ImGui::Begin("Test");
            ImGui::Text("Number of tests");
            ImGui::SameLine();
            ImGui::InputInt("##numberOfTestsInput", &numberOfTests);
            ImGui::Text("Diamond-Square Algorithm");
            ImGui::SameLine();
            if (ImGui::Button("Test 1")) {
                map.resize(mapSize);
                for (int i = 0; i < mapSize; i++) {
                    map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
                }
                terrain.setSize(mapSize);
                mapBuilder.setOffset(meshPointDistance);
                terrain.setRoughness(roughness);
                terrain.setFlattening(flatteningScale);
                terrain.setSize(mapSize);
                genTimer.measureDiamondSquare(numberOfTests);
            }
            ImGui::Text("Perlin Noise Algorithm");
            ImGui::SameLine();
            if (ImGui::Button("Test 2")) {
                map.resize(mapSize);
                for (int i = 0; i < mapSize; i++) {
                    map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
                }
                terrain.setSize(mapSize);
                mapBuilder.setOffset(meshPointDistance);
                genTimer.measurePerlin(numberOfTests, baseFrequency, octaves, persistance, noiseAmplitude);
            }
            ImGui::End();*/
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            /* Swap front and back buffers */
            glfwSwapBuffers(window);

            /* Poll for and process events */
            glfwPollEvents();
        }
        
    }
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
    return 0;
}