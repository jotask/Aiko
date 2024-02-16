#version 330

#define MAX_NUM_TOTAL_PARTICLES 1000

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform float radius;
uniform vec2 resolution;
uniform int nParticles;
uniform vec2 particles[MAX_NUM_TOTAL_PARTICLES];

void main()
{
    // Calculate the aspect ratio
    float aspectRatio = resolution.x / resolution.y;

    // Initialize color as black
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

    // Loop through the particles
    for (int i = 0; i < min(nParticles, MAX_NUM_TOTAL_PARTICLES); i++) {
        // Transform world coordinates to clip space
        vec4 clipPosition = u_mvpMatrix * vec4(particles[i], 1.0);

        // Perspective divide to get normalized device coordinates
        vec2 ndc = clipPosition.xy / clipPosition.w;

        // Convert to screen space
        vec2 fragCoord = (ndc + 1.0) * 0.5 * resolution;
        fragCoord.x *= aspectRatio; // Scale X coordinate to compensate for aspect ratio

        // Calculate the distance from the current fragment to the center of the circle
        float distanceToCenter = distance(fragCoord, gl_FragCoord.xy);

        // Check if the fragment is inside the circle
        if (abs(distanceToCenter - radius) < 0.02) {
            // Fragment is inside the circle, set the color to white
            color = vec4(1.0, 1.0, 1.0, 1.0);
            // You can add break; here if you want to stop checking further circles
        }
    }

    // Set the final color
    finalColor = color;
}