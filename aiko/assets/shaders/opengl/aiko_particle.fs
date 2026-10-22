#version 330

#define MAX_NUM_TOTAL_PARTICLES 1000

out vec4 finalColor;

uniform vec2 iResolution;
uniform float radius;
uniform int nParticles;
uniform vec2 particles[MAX_NUM_TOTAL_PARTICLES];

void main()
{
    // float distanceFromCenter = length(gl_PointCoord - vec2(0.5, 0.5)); // Calculate distance from the center of the point sprite
    // if (distanceFromCenter <= radius) {
    //     vec2 uv = gl_FragCoord.xy / iResolution.xy;
    //     finalColor = vec4(uv.x, uv.y, 0.0, 1.0); // Should show a gradient from black to red (x) and black to green (y)
// 
    // } else {
    //     // discard; // Discard the fragment if outside the radius
    // }
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    finalColor = vec4(uv.x, uv.y, 0.0, 1.0); // Should show a gradient from black to red (x) and black to green (y)

}