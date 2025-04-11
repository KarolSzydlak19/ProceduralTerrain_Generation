#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// The shadow map passed in as a sampler
uniform sampler2D shadowMap;

void main() {
    // Sample the shadow map
    float depthValue = texture(shadowMap, TexCoord).r; // Access the depth value from the shadow map
    
    // Display the depth value in grayscale (depth ranges from 0 to 1)
    FragColor = vec4(vec3(depthValue), 1.0);
}
