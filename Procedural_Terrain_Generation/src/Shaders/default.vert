#version 330 core

layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec2 aTex;      // Texture coordinates
layout(location = 2) in vec3 aTangent;  // Tangent vector
layout(location = 3) in vec3 aNormal;   // Normal vector

uniform mat4 camMatrix;  // Camera transformation matrix
uniform mat4 model;      // Model matrix combining translation, rotation, and scaling
uniform mat4 lightProjection;  // Projection matrix for shadow mapping

out vec3 currPos;
out vec2 texCoord;   // Pass texture coordinates to fragment shader
out vec3 tangent;    // Pass tangent to fragment shader
out vec3 normal;     // Pass normal to fragment shader
out float height;    // Pass height information to fragment shader
out vec3 fragPos;
out vec4 fragPosLight;

void main()
{
    // Apply model transformation to vertex position
    vec4 worldPos = model * vec4(aPos, 1.0f);
    
    // Pass transformed position to the fragment shader
    currPos = worldPos.xyz;
    fragPos = worldPos.xyz;  // For lighting calculations

    // Apply camera transformation and projection to calculate final position
    gl_Position = camMatrix * worldPos;

    // Pass additional data to the fragment shader
    texCoord = aTex;  // Texture coordinates
    tangent = aTangent;  // Tangent
    normal = mat3(model) * aNormal;  // Transform the normal by the model matrix (no translation)
    height = aPos.y;  // The height of the vertex for custom shaders/effects
    
    // Calculate position in light's projection space for shadow mapping
    fragPosLight = lightProjection * worldPos;
}
