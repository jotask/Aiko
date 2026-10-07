#version 330

#define S(a,b,t) smoothstep(a,b,t)

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
    // Normalized coordinates of the pixel, from -1 to 1
    vec2 uv = (gl_FragCoord.xy * 2.0 - iResolution.xy) / min(iResolution.x, iResolution.y);
    
    // Center of the screen will be the center of the circle
    vec2 center = vec2(0.0, 0.0);
    
    // Radius of the circle, you can modify this or make it dynamic with iTime
    float radius = 0.5f;
    
    // Calculate the distance from the current fragment to the center
    float dist = distance(uv, center);
    
    // Draw the circle
    if(dist < radius)
    {
        // Inside the circle
        fragColor = vec4(1.0, 1.0, 1.0, 1.0); // White circle
    }
    else
    {
        // Outside the circle
        fragColor = vec4(0., 0.0, 0.0, 1.0); // Black background
    }

}
