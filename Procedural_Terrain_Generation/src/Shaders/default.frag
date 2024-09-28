#version 330 core

out vec4 FragColor;

in vec2 texCoord;  // Interpolated texture coordinates from vertex shader
in vec3 tangent;   // Interpolated tangent from vertex shader
in vec3 normal;    // Interpolated normal from vertex shader
in float height;   // Interpolated height from vertex shader
in vec3 fragPos;
in vec4 fragPosLight;

//texture blending
uniform float snowMin;
uniform float snowMax;
uniform float rockMin;
uniform float rockMax;
uniform float grassMin;
uniform float grassMax;
uniform float soilMin;
uniform float soilMax;
uniform float maxTextureNoise;

//uniform float snowHeight;
//uniform float rockHeight;
//uniform float grassHeight;
//uniform float soilHeight;
uniform bool useSnowTex;
uniform bool useGrassTex;
uniform bool useRockTex;
uniform bool useSoilTex;

uniform sampler2D noiseTexture;  // Noise texture
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

// NOISE FUNCTION
float randomNoise(vec2 coords) {
    // Use a basic random formula (this can be replaced by more complex noise later)
    return fract(sin(dot(coords.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

// Function to calculate texture blending based on height
vec4 blendTextures(float height, vec2 texCoord) {
    // Sample noise value
    float noiseValue = texture(noiseTexture, texCoord).r; // red channel value
    noiseValue = (noiseValue * 2.0 - 1.0) * maxTextureNoise;
    //height += noiseValue;
    // Initialize texture weights
    float snowWeight = 0.0;
    float rockWeight = 0.0;
    float grassWeight = 0.0;
    float soilWeight = 0.0;

    // Compute weights only if height is within the texture's range
    if (useSnowTex && height >= snowMin && height <= snowMax) {
        float snowMid = (snowMin + snowMax) * 0.5;
         snowWeight = smoothstep(snowMin, snowMax, height);
    }
    if (useRockTex && height >= rockMin && height <= rockMax) {
        float rockMid = (rockMin + rockMax) * 0.5;
        rockWeight = smoothstep(rockMin, rockMax, height);
    }
    if (useGrassTex && height >= grassMin && height <= grassMax) {
        float grassMid = (grassMin + grassMax) * 0.5;
         grassWeight = smoothstep(grassMin, grassMax, height);
    }
    if (useSoilTex && height >= soilMin && height <= soilMax) {
        float soilMid = (soilMin + soilMax) * 0.5;
        soilWeight = smoothstep(soilMin, soilMax, height);
    }

    // Calculate total weight for normalization
    float totalWeight = snowWeight + rockWeight + grassWeight + soilWeight;

    // If total weight is zero (no textures in range), find the closest texture
if (totalWeight == 0.0) {
    float closestDistance = 1e6;  // Initialize with a very large distance
    vec4 closestTexture = vec4(0.0, 0.0, 0.0, 1.0);  // Default to black if no texture is found

    // Check distance to snow texture range
    if (useSnowTex) {
        float snowDist = min(abs(height - snowMin), abs(height - snowMax));
        if (snowDist < closestDistance) {
            closestDistance = snowDist;
            closestTexture = texture(snowTexture, texCoord);
        }
    }

    // Check distance to rock texture range
    if (useRockTex) {
        float rockDist = min(abs(height - rockMin), abs(height - rockMax));
        if (rockDist < closestDistance) {
            closestDistance = rockDist;
            closestTexture = texture(stoneTexture, texCoord);
        }
    }

    // Check distance to grass texture range
    if (useGrassTex) {
        float grassDist = min(abs(height - grassMin), abs(height - grassMax));
        if (grassDist < closestDistance) {
            closestDistance = grassDist;
            closestTexture = texture(grassTexture, texCoord);
        }
    }

    // Check distance to soil texture range
    if (useSoilTex) {
        float soilDist = min(abs(height - soilMin), abs(height - soilMax));
        if (soilDist < closestDistance) {
            closestDistance = soilDist;
            closestTexture = texture(soilTexture, texCoord);
        }
    }

    return closestTexture;  // Return the closest texture found (or black if none is found)
}

    // Normalize the weights
    snowWeight /= totalWeight;
    rockWeight /= totalWeight;
    grassWeight /= totalWeight;
    soilWeight /= totalWeight;

    // Sample each texture
    vec4 snowColor = texture(snowTexture, texCoord);
    vec4 rockColor = texture(stoneTexture, texCoord);
    vec4 grassColor = texture(grassTexture, texCoord);
    vec4 soilColor = texture(soilTexture, texCoord);

    // Blend the colors based on the normalized weights
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
