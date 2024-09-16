#version 330 core

out vec3 texCoords;

unifrom samplerCube skybox;

void main() {
	FragColor = texture(skybox, texCoords);
}