#version 330 core

out vec4 FragColor;

in vec2 texCoord;  // Interpolated texture coordinates from vertex shader
in vec3 tangent;   // Interpolated tangent from vertex shader
in vec3 normal;    // Interpolated normal from vertex shader
in float height;   // Interpolated height from vertex shader
in vec3 fragPos;
in vec4 fragPosLight;

uniform sampler2D snowTexture;   // Snow texture
uniform sampler2D grassTexture;  // Grass texture
uniform sampler2D soilTexture;   // Soil texture
uniform sampler2D stoneTexture;  // Stone texture
uniform sampler2D normalMap;     // Normal map texture
uniform sampler2D shadowMap;

uniform vec3 sunDirection;        // Position of the sun in world space
uniform vec3 lightColor;         // Color of the light
uniform vec3 viewPos;            // Position of the camera/viewer

float defaultAmbient = 0.15f;
float shadow = 0.0f;
vec3 lightCoords = fragPosLight.xyz / fragPosLight.w;

// Function to calculate texture blending based on height
vec4 blendTextures(float height, vec2 texCoord) {
    // Define height thresholds for different textures
    float snowHeight = 300.0;
    float rockHeight = 100.0;
    float grassHeight = -1000000.0;

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

    // Blend the colors based on the calculated weights
    return snowColor * snowWeight + rockColor * rockWeight + grassColor * grassWeight + soilColor * soilWeight;
}

vec4 directLight(vec4 textureColor, vec3 normal, vec3 lightDirection, vec3 viewPos, vec3 fragPos, vec3 lightColor) {
    float ambientStrength = 0.15f;
    float specularStrength = 0.50f;

    // Normalize the inputs
    vec3 norm = normalize(normal);
    vec3 lightDir = normalize(lightDirection);
    vec3 viewDir = normalize(viewPos - fragPos);

    // Ambient lighting
    vec3 ambient = ambientStrength * textureColor.rgb;

    // Diffuse lighting
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor * textureColor.rgb;

    // Specular lighting
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), 16.0f);
    vec3 specular = spec * specularStrength * textureColor.rgb;

    // Combine the results
    vec3 lighting = ambient + diffuse + specular;

    return vec4(lighting, textureColor.a);  // Return the color with the alpha of the texture
}

void main() {
    // Normal mapping
    vec3 normalFromMap = texture(normalMap, texCoord).rgb;
    normalFromMap = normalFromMap * 2.0 - 1.0;  // Convert from [0,1] to [-1,1]

    // Transform normal from tangent to world space
    vec3 T = normalize(tangent);
    vec3 N = normalize(normal);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    vec3 worldNormal = normalize(TBN * normalFromMap);

    // Use the separate blendTextures function to blend textures based on height
    vec4 blendedColor = blendTextures(height, texCoord);

    // Define light direction (ensure it's normalized)
    vec3 lightDir = normalize(-sunDirection);

    // Apply direct lighting using your function
    vec4 litColor = directLight(blendedColor, worldNormal, lightDir, viewPos, fragPos, lightColor);

    // Shadow mapping
    if (lightCoords.z <= 1.0f) {
        //lightCoords = (lightCoords + 1.0f) / 2.0f;

        float closestDepth = texture(shadowMap, lightCoords.xy).r;
        float currentDepth = lightCoords.z;
        float bias = max(0.05 * (1.0 - dot(worldNormal, lightDir)), 0.005);
        if (currentDepth - bias > closestDepth) {
            shadow = 1.0f;
        }
        
    }
    // Apply shadow to the lit color
    vec4 finalColor = litColor * (1.0f - shadow);

    FragColor = finalColor;  // Output the final color
}
