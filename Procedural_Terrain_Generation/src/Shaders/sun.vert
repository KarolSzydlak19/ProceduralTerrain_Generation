#version 330 core

layout(location = 0) in vec3 aPos;  // Position of the vertex
layout(location = 1) in vec2 aTex;  // Texture coordinates

uniform mat4 sunCamMatrix;  // Camera matrix (view + projection)
uniform mat4 model;      // Model matrix (for transformations like translation, scaling, etc.)

out vec2 texCoord;  // Texture coordinates passed to the fragment shader

void main()
{
    // Apply the model transformation, then the camera (view + projection) transformation
    gl_Position = sunCamMatrix * model * vec4(aPos, 1.0);
    texCoord = aTex;
}
