#version 330 core

out vec4 FragColor;

uniform vec4 color;

// Ambient light color and intensity
const vec3 ambientLightColor = vec3(1.0);
const float ambientLightIntensity = 0.2; // Adjust as needed

// Ambient occlusion factor
const float ambientOcclusion = 0.5; // Adjust as needed

void main()
{
    // Calculate ambient light contribution
    vec3 ambientLight = ambientLightColor * ambientLightIntensity;

    // Calculate ambient occlusion contribution
    vec3 ambientOcclusionColor = vec3(1.0) * ambientOcclusion;

    // Final color with ambient light and ambient occlusion
    vec3 finalColor = (color.rgb * ambientOcclusionColor) + ambientLight;

    // Output final color
    FragColor = vec4(finalColor, color.a);
}
