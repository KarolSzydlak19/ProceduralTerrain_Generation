#version 330 core

layout(location = 0) in vec3 aPos;      // Vertex position
layout(location = 1) in vec2 aTex;      // Texture coordinates
layout(location = 2) in vec3 aTangent;  // Tangent vector
layout(location = 3) in vec3 aNormal;   // Normal vector

uniform mat4 camMatrix;  // Camera transformation matrix

out vec2 texCoord;   // Pass texture coordinates to fragment shader
out vec3 tangent;    // Pass tangent to fragment shader
out vec3 normal;     // Pass normal to fragment shader
out float height;    // Pass height information to fragment shader
out vec3 fragPos;

void main()
{
    gl_Position = camMatrix * vec4(aPos, 1.0);  // Apply camera matrix transformation
    texCoord = aTex;  // Pass texture coordinates to fragment shader
    tangent = aTangent;  // Pass tangent to fragment shader
    normal = aNormal;  // Pass normal to fragment shader
    height = aPos.y;  // Store the height of the vertex (y-coordinate)
    fragPos = aPos;
}
