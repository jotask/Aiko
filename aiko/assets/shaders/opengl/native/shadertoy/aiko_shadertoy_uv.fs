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
    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    // Color based on position to verify coordinates
    fragColor = vec4(uv.x, uv.y, 0.0, 1.0); // Should show a gradient from black to red (x) and black to green (y)
}
