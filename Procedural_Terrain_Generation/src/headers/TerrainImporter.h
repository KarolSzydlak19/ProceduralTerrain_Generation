#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <Windows.h>

class TerrainImporter {
public:
	TerrainImporter();
	bool importTerrainFromOBJ(const std::string& filename,
        std::vector<glm::vec3>& outVertices,
        std::vector<glm::vec2>& outTexCoords,
        std::vector<glm::vec3>& outNormals,
        std::vector<unsigned int>& outIndices);
    std::string TerrainImporter::showOpenFileDialog(const std::string& filter);
private:
    void processVertex(const std::string& vertexData, std::vector<unsigned int>& indices);

};