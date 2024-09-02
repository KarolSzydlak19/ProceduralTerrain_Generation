#version 330 core

out vec4 FragColor;

in vec2 texCoord;  // Interpolated texture coordinates from vertex shader
in vec3 tangent;   // Interpolated tangent from vertex shader
in float height;   // Interpolated height from vertex shader

uniform sampler2D snowTexture;   // Snow texture
uniform sampler2D grassTexture;  // Grass texture
uniform sampler2D soilTexture;   // Soil texture
uniform sampler2D stoneTexture;  // Stone texture
uniform sampler2D normalMap;     // Normal map texture

void main()
{
    // Define height thresholds for different textures
    float snowHeight = 400.0;
    float rockHeight = 200.0;
    float grassHeight = -200.0;

    // Calculate weights for each texture based on height
    float snowWeight = smoothstep(rockHeight, snowHeight, height);
    float rockWeight = smoothstep(grassHeight, rockHeight, height) * (1.0 - snowWeight);
    float grassWeight = smoothstep(0.0, grassHeight, height) * (1.0 - rockWeight) * (1.0 - snowWeight);
    float soilWeight = 1.0 - (snowWeight + rockWeight + grassWeight);

    // Sample each texture
    vec4 snowColor = texture(snowTexture, texCoord);
    vec4 rockColor = texture(stoneTexture, texCoord);
    vec4 grassColor = texture(grassTexture, texCoord);
    vec4 soilColor = texture(soilTexture, texCoord);

    // Normal mapping
    vec3 normalFromMap = texture(normalMap, texCoord).rgb;
    normalFromMap = normalize(normalFromMap * 2.0 - 1.0);  // Convert from [0,1] to [-1,1]

    // Blend the colors based on the calculated weights
    vec4 finalColor = snowColor * snowWeight + rockColor * rockWeight + grassColor * grassWeight + soilColor * soilWeight;

    FragColor = finalColor;  // Output the final color
}
