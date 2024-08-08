#version 330 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D mapSurfaceTexture;
void main()
{
    FragColor = texture(mapSurfaceTexture, texCoord);
}
