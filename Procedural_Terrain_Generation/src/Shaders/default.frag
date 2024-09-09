#version 330 core

out vec4 FragColor;

in vec2 texCoord;  // Interpolated texture coordinates from vertex shader
in vec3 tangent;   // Interpolated tangent from vertex shader
in vec3 normal;    // Interpolated normal from vertex shader
in float height;   // Interpolated height from vertex shader
in vec3 fragPos;

uniform sampler2D snowTexture;   // Snow texture
uniform sampler2D grassTexture;  // Grass texture
uniform sampler2D soilTexture;   // Soil texture
uniform sampler2D stoneTexture;  // Stone texture
uniform sampler2D normalMap;     // Normal map texture

uniform vec3 sunDirection;        // Position of the sun in world space
uniform vec3 lightColor;         // Color of the light
uniform vec3 viewPos;            // Position of the camera/viewer

float defaultAmbient = 0.15f;
void main()
{
    // Define height thresholds for different textures
    float snowHeight = 200.0;
    float rockHeight = -10000.0;
    float grassHeight = -1000000.0;

    //lighting constants:
    float lightIntensity = 3.0f;

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
    normalFromMap = normalFromMap * 2.0 - 1.0;  // Convert from [0,1] to [-1,1]

    // Transform normal from tangent to world space
    vec3 T = normalize(tangent);
    vec3 N = normalize(normal);
    vec3 B = cross(N, T);
    mat3 TBN = mat3(T, B, N);
    vec3 worldNormal = normalize(TBN * normalFromMap);

    // Lighting calculations
    vec3 finalLightColor = lightColor * lightIntensity;
    //vec3 lightDir = normalize(sunDirection - fragPos);  // Calculate light direction
    vec3 lightDir = normalize(-sunDirection);
    vec3 ambient = defaultAmbient * finalLightColor;  // Ambient lighting
    float diff = max(dot(worldNormal, lightDir), 0.0f);
    vec3 diffuse = diff * finalLightColor;  // Diffuse lighting

    vec3 viewDir = normalize(viewPos - fragPos);  // Direction to the viewer
    vec3 reflectDir = reflect(-lightDir, worldNormal);  // Reflection direction
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);  // Specular lighting
    vec3 specular = spec * finalLightColor;

    vec3 lighting = (ambient + diffuse + specular);  // Combine lighting components

    // Blend the colors based on the calculated weights
    vec4 finalColor = (snowColor * snowWeight + rockColor * rockWeight + grassColor * grassWeight + soilColor * soilWeight) * vec4(lighting, 1.0);

    FragColor = finalColor;  // Output the final color
}
