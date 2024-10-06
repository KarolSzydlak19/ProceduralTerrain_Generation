#ifndef TERRAIN_EXPORTER_H
#define TERRAIN_EXPORTER_H
#define NOMINMAX
#include <windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <glm/glm.hpp> 
#include <glad/glad.h>
#include <commdlg.h>
#include <iostream>

class TerrainExporter {
private:
    float exportingProgress;
    std::string exportingState;
public:
    TerrainExporter();
    float getExportingProgress();
    std::string getExportingState();
    void exportTerrainToOBJ(const std::string& filename,
        const GLfloat* interleavedVertices,
        int vertexCount,
        const std::vector<glm::vec3>& normals,
        const GLuint* indices,
        int indexCount);
    std::string showSaveFileDialog(const std::string& defaultFileName);
};
#endif
