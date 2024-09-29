#include "Map.h"
#include <Shader.h>

Map::Map(int size, float roughness, float flatteningScale, float maxTextureNoise, std::vector<std::vector<glm::vec3>>& map, MapBuilder& mapBuilder, PerlinNoiseGenerator noiseGen)
    : size(size), roughness(roughness), flatteningScale(flatteningScale), maxTextureNoise(maxTextureNoise), map(map), mapBuilder(mapBuilder), noiseGen(noiseGen) {
    // Initialize the corners
    map[0][0].y = randomOffset(roughness);
    map[0][size - 1].y = randomOffset(roughness);
    map[size - 1][0].y = randomOffset(roughness);
    map[size - 1][size - 1].y = randomOffset(roughness);
    //generateNoiseMap(1024, 0.05f, 6, 0.5f, 1.0f);
    //generateNoiseTexture();
    mapVAO = nullptr;
    mapEBO = nullptr;
    mapVBO = nullptr;
    maxY = minY = 0;
}

float Map::getMinY() {
    return minY;
}

float Map::getMaxY() {
    return maxY;
}

void Map::setRoughness(float val) {
    roughness = val;
}

void Map::setFlattening(float val) {
    flatteningScale = val;
}

void Map::setSize(int mapSize) {
    size = mapSize;
}

void Map::setTexNoise(float val) {
    maxTextureNoise = val;
}

void Map::searchEdgeValues() {

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (map[i][j].y < minY) {
                minY = map[i][j].y;
            }
            if (map[i][j].y > maxY) {
                maxY = map[i][j].y;
            }
        }
    }
}

void Map::generateNoiseMap(int size, float baseFrequency, int octaves, float persistence, float maxNoiseAmplitude) {
    // Resize the noiseMap to fit the desired size
    noiseMap.resize(size, std::vector<float>(size, 0.0f));

    // Initialize the Perlin noise generator
    PerlinNoiseGenerator noiseGen;

    // Iterate through each point in the map
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            // Calculate the position in noise space using baseFrequency
            float x = i * baseFrequency;
            float z = j * baseFrequency;

            // Generate fractal noise for the current point
            float noiseValue = noiseGen.fractalNoise(x, z, octaves, persistence, baseFrequency, maxNoiseAmplitude);

            // Optionally, clamp the result to [0, 1], but apply amplitude scaling
            noiseMap[i][j] = noiseValue * maxNoiseAmplitude;

            // Set the height (y) for your terrain using the noise value
            map[i][j].y = noiseMap[i][j];  // Adjust height using noise and amplitude scaling
        }
    }
}

void Map::generateWithPerlin() {
    generateNoiseMap(size, 0.05f, 6, 0.5f, roughness);  // Customize parameters as needed
    //initAxes();
}

void Map::generateNoiseTexture() {
   /* std::cout << "Before tex gen" << std::endl;
    std::vector<float> flatNoiseMap(size * size);
    std::cout << "after tex gen" << std::endl;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            flatNoiseMap[i * size + j] = noiseMap[i][j];
        }
    }
    std::cout << "after tex gen" << std::endl;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, size, size, 0, GL_RED, GL_FLOAT, flatNoiseMap.data());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    std::cout << "after tex gen" << std::endl;*/
}

void Map::uploadNoiseTexture(Shader& shader) {
    //std::cout << "NOOOOOOOOOOOIIIIIIIIIIIIIIIIIISSSSSSSSSSEEEEEEEEEEEEEEEEEEE " << shader.ID << noiseTexture << std::endl;
    glActiveTexture(GL_TEXTURE1);  
    glBindTexture(GL_TEXTURE_2D, noiseTexture);  
    shader.Activate();
    glUniform1i(glGetUniformLocation(shader.ID, "noiseTexture"), noiseTexture);
}

float Map::randomOffset(float range) {
    return ((rand() % 100) / 100.0f) * range * 2 - range;
}
void Map::Diamond_step(int x, int y, int stepSize, float scale) {
    mapBuilder.Diamond_step(x, y, stepSize, scale, size);
}

void Map::Square_step(int x, int y, int halfStep, float scale) {
    mapBuilder.Square_step(x, y, halfStep, scale, size);
}

void Map::generate() {
    //print();
    std::cout << size << std::endl;
    int stepSize = size - 1;
    stepSize /= 2;
    float scale = roughness;
    while (stepSize > 1) {
        int halfStep = stepSize / 2;
        // Diamond step
        for (int x = 0; x < size - 1; x += stepSize) {
            for (int y = 0; y < size - 1; y += stepSize) {
                mapBuilder.Diamond_step(x, y, stepSize, scale, size);
            }
        }
        // Square step
        for (int x = 0; x < size; x += halfStep) {
            for (int y = (x + halfStep) % stepSize; y < size; y += stepSize) {
                mapBuilder.Square_step(x, y, halfStep, scale, size);
            }
        }

        stepSize /= 2;
        scale /= flatteningScale;
    }
    searchEdgeValues();
    //std::cout << "AFTER" << std::endl;
    //print();
}

void Map::initAxes() {
    mapBuilder.initAxes(size);
}

void Map::generateVertices() {
    int vertexCount = size * size;
    vertices = new GLfloat[vertexCount * 5]; // Each vertex has 5 components (x, y, z) + texture coordinates (xTex, yTex)
    mapBuilder.generateVertices(vertices, size);
}

void Map::generateIndices() {
    int indexCount = (size - 1) * (size - 1) * 6; // 6 indices per square (2 triangles)
    indices = new GLuint[indexCount];
    mapBuilder.generateIndices(indices, size);
}

void checkOpenGLError(const std::string& stmt) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        std::cerr << "OpenGL error: " << err << " after " << stmt << std::endl;
    }
}

void Map::generateTangents() {
    tangents.resize(size * size, glm::vec3(0.0f));
    // Tangent calculation logic based on vertex positions and texture coordinates
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - 1; ++j) {
            // Vertex positions
            glm::vec3 v0 = map[i][j];
            glm::vec3 v1 = map[i + 1][j];
            glm::vec3 v2 = map[i][j + 1];

            // Texture coordinates (assuming they're part of your vertices)
            glm::vec2 uv0(vertices[(i * size + j) * 5 + 3], vertices[(i * size + j) * 5 + 4]);
            glm::vec2 uv1(vertices[((i + 1) * size + j) * 5 + 3], vertices[((i + 1) * size + j) * 5 + 4]);
            glm::vec2 uv2(vertices[(i * size + j + 1) * 5 + 3], vertices[(i * size + j + 1) * 5 + 4]);

            // Edge vectors
            glm::vec3 deltaPos1 = v1 - v0;
            glm::vec3 deltaPos2 = v2 - v0;
            glm::vec2 deltaUV1 = uv1 - uv0;
            glm::vec2 deltaUV2 = uv2 - uv0;

            float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
            glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;

            // Store tangent for each vertex of the triangle
            tangents[i * size + j] += tangent;
            tangents[(i + 1) * size + j] += tangent;
            tangents[i * size + j + 1] += tangent;
        }
    }

    // Normalize all tangents
    for (auto& tangent : tangents) {
        tangent = glm::normalize(tangent);
    }
}

void Map::generateNormals() {
    // Initialize normals with zeros
    normals.resize(size * size, glm::vec3(0.0f, 0.0f, 0.0f));

    // Loop through the entire grid, calculating face normals
    for (int i = 0; i < size - 1; ++i) {
        for (int j = 0; j < size - 1; ++j) {
            // Get vertices of the current quad (2 triangles per quad)
            glm::vec3 v0 = map[i][j];            // Bottom-left
            glm::vec3 v1 = map[i + 1][j];        // Bottom-right
            glm::vec3 v2 = map[i][j + 1];        // Top-left
            glm::vec3 v3 = map[i + 1][j + 1];    // Top-right

            // Triangle 1 (v0, v1, v2)
            glm::vec3 U1 = v1 - v0;
            glm::vec3 V1 = v2 - v0;
            glm::vec3 normal1 = glm::normalize(glm::cross(U1, V1));

            // Triangle 2 (v1, v3, v2)
            glm::vec3 U2 = v3 - v1;
            glm::vec3 V2 = v2 - v1;
            glm::vec3 normal2 = glm::normalize(glm::cross(U2, V2));

            // Add the normals to the corresponding vertices
            normals[i * size + j] += normal1;           // v0
            normals[(i + 1) * size + j] += normal1;     // v1
            normals[i * size + (j + 1)] += normal1;     // v2

            normals[(i + 1) * size + j] += normal2;     // v1
            normals[(i + 1) * size + (j + 1)] += normal2; // v3
            normals[i * size + (j + 1)] += normal2;     // v2
        }
    }

    // Normalize all vertex normals (after accumulating)
    for (auto& normal : normals) {
        normal = glm::normalize(normal);
    }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            //normals[i * size + j].x += map[i][j].x;
            //normals[i * size + j].y += map[i][j].y;
            //normals[i * size + j].z += map[i][j].z;
        }
    }
    //printNormals();
}
void Map::generateNormalLines() {
    float normalScale = 10.0f;  // Scale factor for normal line length
    normalLines.clear();

    for (int i = 0; i < size * size; ++i) {
        glm::vec3 vertex = glm::vec3(vertices[i * 5], vertices[i * 5 + 1], vertices[i * 5 + 2]);
        glm::vec3 normalEnd = vertex + normals[i] * normalScale;  // End point of the normal line

        // Store the start and end points of the normal as two vertices
        normalLines.push_back(vertex);
        normalLines.push_back(normalEnd);
    }
}

void Map::printNormals() {
    std::cout << "Printing vertices and normals:" << std::endl;
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            // Get the vertex position
            glm::vec3 vertex = map[i][j];

            // Get the normal corresponding to this vertex
            glm::vec3 normal = normals[i * size + j];

            // Print the vertex position and normal vector
            std::cout << "Vertex at (" << i << ", " << j << "): "
                << vertex.x << ", " << vertex.y << ", " << vertex.z << std::endl;
            std::cout << "Normal: "
                << normal.x << ", " << normal.y << ", " << normal.z << std::endl;
        }
    }
}


void Map::initNormalObjects() {
    generateNormalLines();  // Make sure to generate the normal lines first

    // Generate VAO/VBO for normal lines
    glGenVertexArrays(1, &normalVAO);
    glGenBuffers(1, &normalVBO);

    glBindVertexArray(normalVAO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normalLines.size() * sizeof(glm::vec3), &normalLines[0], GL_STATIC_DRAW);

    // Vertex positions (3 floats per vertex)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);
}

void Map::drawNormals() {
    glBindVertexArray(normalVAO);
    glDrawArrays(GL_LINES, 0, normalLines.size());
    glBindVertexArray(0);
}


void Map::initObjects() {
    mapVAO = new VAO();
    initAxes();
    generateVertices();
    generateIndices();
    generateTangents();
    generateNormals();

    //generateNormalLines();
    //initNormalObjects();
    mapVAO->Bind();

    mapVBO = new VBO(vertices, size * size * 5 * sizeof(GLfloat));

    mapEBO = new EBO(indices, (size - 1) * (size - 1) * 6 * sizeof(GLuint));

    mapVAO->LinkAttrib(*mapVBO, 0, 3, GL_FLOAT, 5 * sizeof(GLfloat), (void*)0);

    mapVAO->LinkAttrib(*mapVBO, 1, 2, GL_FLOAT, 5 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    glGenBuffers(1, &tangentVBO);
    glBindBuffer(GL_ARRAY_BUFFER, tangentVBO);
    glBufferData(GL_ARRAY_BUFFER, tangents.size() * sizeof(glm::vec3), &tangents[0], GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(2);

    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(3); // Normal attribute

    mapVAO->Unbind();
    mapEBO->Unbind();
    mapVBO->Unbind();
}

void Map::cleanUpObjects() {
    // Delete dynamically allocated memory for vertices and indices
    if (vertices != nullptr) {
        delete[] vertices;
        vertices = nullptr;
    }

    if (indices != nullptr) {
        delete[] indices;
        indices = nullptr;
    }

    // Delete the VAO, VBO, and EBO
    if (mapVAO != nullptr) {
        mapVAO->Unbind();
        mapVAO->Delete();  
        delete mapVAO;
        mapVAO = nullptr;
    }

    if (mapVBO != nullptr) {
        mapVBO->Unbind();
        mapVBO->Delete(); 
        delete mapVBO;
        mapVBO = nullptr;
    }

    if (mapEBO != nullptr) {
        mapEBO->Unbind();
        mapEBO->Delete(); 
        delete mapEBO;
        mapEBO = nullptr;
    }

    // Delete normal lines buffer
    if (glIsBuffer(normalVBO)) {
        glDeleteBuffers(1, &normalVBO);
    }

    // Delete tangent buffer
    if (glIsBuffer(tangentVBO)) {
        glDeleteBuffers(1, &tangentVBO);
    }

    // Delete VAO and VBO for normal lines
    if (glIsVertexArray(normalVAO)) {
        glDeleteVertexArrays(1, &normalVAO);
    }
    if (glIsBuffer(normalVBO)) {
        glDeleteBuffers(1, &normalVBO);
    }
}


void Map::draw() {
    int indexCount = (size - 1) * (size - 1) * 6;
    mapVAO->Bind();
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    mapVAO->Unbind();

}

void Map::print() {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            std::cout << map[i][j].x << " " << map[i][j].y << " " << map[i][j].z << std::endl;
        }
    }
}