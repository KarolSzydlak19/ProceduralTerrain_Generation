#include "TerrainImporter.h"

TerrainImporter::TerrainImporter() {}

bool TerrainImporter::importTerrainFromOBJ(const std::string& filename,
    std::vector<glm::vec3>& outVertices,
    std::vector<glm::vec2>& outTexCoords,
    std::vector<glm::vec3>& outNormals,
    std::vector<unsigned int>& outIndices) {
    std::ifstream objFile(filename);
    if (!objFile.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(objFile, line)) {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v") {
            // Vertex position
            glm::vec3 vertex;
            iss >> vertex.x >> vertex.y >> vertex.z;
            outVertices.push_back(vertex);
        }
        else if (prefix == "vt") {
            // Texture coordinate
            glm::vec2 texCoord;
            iss >> texCoord.x >> texCoord.y;
            outTexCoords.push_back(texCoord);
        }
        else if (prefix == "vn") {
            // Normal vector
            glm::vec3 normal;
            iss >> normal.x >> normal.y >> normal.z;
            outNormals.push_back(normal);
        }
        else if (prefix == "f") {
            // Face elements
            std::string vertexData;
            while (iss >> vertexData) {
                processVertex(vertexData, outIndices);
            }
        }
    }

    objFile.close();
    return true;
}

void TerrainImporter::processVertex(const std::string& vertexData, std::vector<unsigned int>& indices) {
    // OBJ format example: "f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3"
    std::istringstream iss(vertexData);
    unsigned int vertexIndex, texCoordIndex, normalIndex;
    char slash;

    iss >> vertexIndex >> slash >> texCoordIndex >> slash >> normalIndex;

    // OBJ file indices are 1-based, but we need them 0-based
    indices.push_back(vertexIndex - 1);
}

std::string TerrainImporter::showOpenFileDialog(const std::string& filter) {
    // Initialize the OPENFILENAME structure
    OPENFILENAME ofn;       // Common dialog box structure
    char szFile[MAX_PATH] = { 0 };  // Buffer for file name

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;  // If you have a window handle, pass it here
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter.c_str();  // Filter options, passed as parameter
    ofn.nFilterIndex = 1;  // Default to the first filter
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;  // Optionally set an initial directory
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;  // Ensures valid path and file exists

    // Display the Open File dialog box
    if (GetOpenFileName(&ofn)) {
        // Return the full file path selected by the user
        return std::string(szFile);  // Use the updated szFile here, which now contains the chosen file path
    }
    else {
        // If the user canceled or there was an error, return an empty string
        return std::string();
    }
}