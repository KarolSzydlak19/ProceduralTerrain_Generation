#include "TerrainExporter.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
TerrainExporter::TerrainExporter() {
    exportingProgress = 0.0f;
    exportingState = "";
}

float TerrainExporter::getExportingProgress() {
    return exportingProgress;
}

std::string TerrainExporter::getExportingState() {
    return exportingState;
}

void TerrainExporter::exportTerrainToOBJ(const std::string& filename,
    const GLfloat* interleavedVertices,
    int vertexCount,
    const std::vector<glm::vec3>& normals,
    const GLuint* indices,  // Use GLuint* here for raw array
    int indexCount) {       // Add indexCount to know the length of the array

    //fileName = showSaveFileDialog(filename);
    exportingProgress = 0.0f;
    std::ofstream objFile;
    objFile.open(filename);
    exportingState = "Exporting vertices";

    std::vector<glm::vec3> positions;
    std::vector<glm::vec2> texCoords;

    // Extract positions and texture coordinates
    for (int i = 0; i < vertexCount; ++i) {
        // Positions (x, y, z are the first 3 floats in each vertex)
        positions.push_back(glm::vec3(interleavedVertices[i * 5], interleavedVertices[i * 5 + 1], interleavedVertices[i * 5 + 2]));

        // Texture coordinates (u, v are the next 2 floats in each vertex)
        texCoords.push_back(glm::vec2(interleavedVertices[i * 5 + 3], interleavedVertices[i * 5 + 4]));
    }
    exportingProgress += 0.1f;
    // Export vertices
    for (const auto& vertex : positions) {
        objFile << "v " << vertex.x << " " << vertex.y << " " << vertex.z << "\n";
    }
    exportingProgress += 0.3f;
    exportingState = "Exporting texture coordinates";
    // Export texture coordinates
    for (const auto& texCoord : texCoords) {
        objFile << "vt " << texCoord.x << " " << texCoord.y << "\n";
    }
    exportingProgress += 0.1f;
    exportingState = "Exporting normals";
    // Export normals
    for (const auto& normal : normals) {
        objFile << "vn " << normal.x << " " << normal.y << " " << normal.z << "\n";
    }
    exportingProgress += 0.49f;
    exportingState = "Exporting indices";
    // Export faces (triangles)
    for (int i = 0; i < indexCount; i += 3) {  // Use indexCount here
        // OBJ is 1-indexed, so add 1 to vertex/texCoord/normal indices
        objFile << "f "
            << indices[i] + 1 << "/" << indices[i] + 1 << "/" << indices[i] + 1 << " "
            << indices[i + 1] + 1 << "/" << indices[i + 1] + 1 << "/" << indices[i + 1] + 1 << " "
            << indices[i + 2] + 1 << "/" << indices[i + 2] + 1 << "/" << indices[i + 2] + 1 << "\n";
    }
    exportingProgress = 1.0f;
    objFile.close();
}


std::string TerrainExporter::showSaveFileDialog(const std::string& defaultFileName) {
    // Initialize the OPENFILENAME structure
    OPENFILENAME ofn;       // Common dialog box structure
    char szFile[MAX_PATH] = { 0 };  // Buffer for file name

    // Initialize file name buffer with the default file name
    strncpy(szFile, defaultFileName.c_str(), sizeof(szFile) - 1);

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = nullptr;  // If you have a window handle, pass it here
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "OBJ Files\0*.obj\0All Files\0*.*\0";  // Filter options
    ofn.nFilterIndex = 1;  // Default to OBJ Files
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;  // You can set an initial directory here if needed
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;  // Ensures valid path and prompts for overwrite

    // Display the Save As dialog box
    if (GetSaveFileName(&ofn)) {
        // Return the full file path selected by the user
        return std::string(szFile);  // Use the updated szFile here, which now contains the chosen file path
    }
    else {
        // If the user canceled or there was an error, return an empty string
        return std::string();
    }
}

void TerrainExporter::exportHeightmap(const std::vector<std::vector<glm::vec3>>& heightMap, const std::string& filename) {
    int mapSize = heightMap.size();
    std::vector<unsigned char> image(mapSize * mapSize);

    // Find min and max height values to normalize the data
    float minHeight = std::numeric_limits<float>::max();
    float maxHeight = std::numeric_limits<float>::min();
    for (int i = 0; i < mapSize; ++i) {
        for (int j = 0; j < mapSize; ++j) {
            float height = heightMap[i][j].y;
            if (height < minHeight) minHeight = height;
            if (height > maxHeight) maxHeight = height;
        }
    }

    // Normalize and populate the image array
    for (int i = 0; i < mapSize; i++) {
        for (int j = 0; j < mapSize; j++) {
            float normalizedHeight = (heightMap[i][j].y - minHeight) / (maxHeight - minHeight);
            image[i * mapSize + j] = static_cast<unsigned char>(normalizedHeight * 255.0f);
        }
    }

    // Save the image using stb_image_write
    stbi_write_png(filename.c_str(), mapSize, mapSize, 1, image.data(), mapSize);
}
