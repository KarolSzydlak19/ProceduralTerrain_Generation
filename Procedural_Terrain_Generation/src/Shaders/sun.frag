#version 330 core
out vec4 FragColor;

in vec2 texCoord;  // Texture coordinates from the vertex shader

uniform sampler2D sunTexture;  // The sun texture sampler

void main()
{
    FragColor = texture(sunTexture, texCoord);  // Apply the sun texture to the fragment
}
