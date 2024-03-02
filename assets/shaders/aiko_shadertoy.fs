#version 330

#define S(a,b,t) smoothstep(a,b,t)

// Input vertex attributes (from vertex shader)
in vec2 fragTexCoord;

// Output fragment color
out vec4 fragColor;

uniform vec3      iResolution;           // viewport resolution (in pixels)
uniform float     iTime;                 // shader playback time (in seconds)
uniform float     iTimeDelta;            // render time (in seconds)
uniform float     iFrameRate;            // shader frame rate
uniform int       iFrame;                // shader playback frame
uniform vec4      iMouse;                // mouse pixel coords. xy: current (if MLB down), zw: click
uniform vec4      iDate;                 // (year, month, day, time in seconds)

void main()
{
    // Normalized time value, will cycle from 0 to 1
    float time = mod(iTime, 1.0);

    // Create a dynamic RGB color using the time variable
    // This will cycle through colors in a smooth manner
    vec3 dynamicColor = vec3(
        sin(time * 2.0 * 3.1415926) * 0.5 + 0.5, // Red channel, cycles from 0 to 1
        cos(time * 2.0 * 3.1415926) * 0.5 + 0.5, // Green channel, cycles from 0 to 1
        sin(time * 4.0 * 3.1415926) * 0.5 + 0.5  // Blue channel, cycles at a different rate
    );

	fragColor = vec4(dynamicColor, 1.0); // Output the color with full alpha

}
