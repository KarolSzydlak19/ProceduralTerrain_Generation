//#include "GuiWindow.h"
//
//GuiWindow::GuiWindow() {}
//
//void GuiWindow::init(GLFWwindow* window) {
//    IMGUI_CHECKVERSION();
//    ImGui::CreateContext();
//    ImGuiIO& io = ImGui::GetIO(); (void)io;
//    ImGui::StyleColorsDark();
//    ImGui_ImplGlfw_InitForOpenGL(window, true);
//    ImGui_ImplOpenGL3_Init("#version 330");
//    ImGui::GetIO().IniFilename = P_DIR;
//}
//
//void GuiWindow::terrain() {
//    ImGui::Begin("Terrain properties");
//    ImGui::Text("Algorithm");
//    ImGui::SameLine();
//    if (ImGui::Combo("##algorithmSelection", &currentAlgorithm, generationTypeLabel, IM_ARRAYSIZE(generationType))) {
//        currentAlgorithm = generationType[currentAlgorithm];
//    }
//    ImGui::Text("Use custom seed");
//    ImGui::SameLine();
//    ImGui::Checkbox("##seedChechbox", &useCustomSeed);
//    ImGui::Text("Custom seed");
//    ImGui::SameLine();
//    ImGui::InputInt("##customSeed", &customSeed);
//    //only in diamond square
//    if (currentAlgorithm == 0) {
//        ImGui::Text("Elevation");
//        ImGui::SameLine();
//        ImGui::InputFloat("##elevation", &roughness, 0.0f, 0.0f, "%.2f");
//        ImGui::Text("Flattening scale");
//        ImGui::SameLine();
//        ImGui::InputFloat("##flatteningScale", &flatteningScale, 0.0f, 0.0f, "%.2f");
//    }
//    else if (currentAlgorithm == 1) {
//        ImGui::Text("Base frequency");
//        ImGui::SameLine();
//        ImGui::SliderFloat("##baseFreqSlider", &baseFrequency, 0.0f, 1.0f, "%.2f");
//        ImGui::Text("Octaves");
//        ImGui::SameLine();
//        ImGui::SliderInt("##octavesSlider", &octaves, 1.0f, 10.0f);
//        ImGui::Text("Persistance");
//        ImGui::SameLine();
//        ImGui::SliderFloat("##persistanceSlider", &persistance, 0.0f, 1.0f, "%.2f");
//        ImGui::Text("Amplitude");
//        ImGui::SameLine();
//        ImGui::InputFloat("##amplitudeInput", &noiseAmplitude, 0.0f, 0.0f, "%.2f");
//    }
//    ImGui::Text("Mesh size");
//    ImGui::SameLine();
//    if (ImGui::Combo("##meshSizeSelection", &currentMeshSizeIndex, allowedSizesLabels, IM_ARRAYSIZE(allowedSizesLabels))) {
//        mapSize = allowedSizes[currentMeshSizeIndex];
//    }
//    ImGui::Text("Mesh offset");
//    ImGui::SameLine();
//    ImGui::InputFloat("#Distance between mesh points", &meshPointDistance, 0.0f, 0.0f, "%.2f");
//    ImGui::Text("Show mesh");
//    ImGui::SameLine();
//    ImGui::Checkbox("##showmeshCheckbox", &enableTriangles);
//    if (ImGui::Button("Generate")) {
//        terrain.cleanUpObjects();
//        bool genFinished = false;
//        ImGui::Text("Generating terrain...");
//        map.resize(mapSize);
//        for (int i = 0; i < mapSize; i++) {
//            map[i].resize(mapSize, { 0.0f, 0.0f, 0.0f });
//        }
//        terrain.setSize(mapSize);
//        mapBuilder.setOffset(meshPointDistance);
//        if (currentAlgorithm == 0) {
//            terrain.setRoughness(roughness);
//            terrain.setFlattening(flatteningScale);
//            terrain.generate();
//        }
//        else if (currentAlgorithm == 1) {
//            terrain.generateWithPerlin(baseFrequency, octaves, persistance, noiseAmplitude);
//        }
//        terrain.searchEdgeValues();
//        terrain.initObjects();
//        minTexHeight = terrain.getMinY();
//        maxTexHeight = terrain.getMaxY();
//    }
//    ImGui::Text("Scale");
//    ImGui::SameLine();
//    ImGui::SliderFloat("##scaleSlider", &terrainScale, 1.0f, 100.0f, "%.2f");
//
//    ImGui::End();
//}