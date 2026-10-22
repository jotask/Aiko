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

vec3 palette( float t) {
    vec3 a = vec3(0.5,0.5,0.5);
    vec3 b = vec3(0.5,0.5,0.5);
    vec3 c = vec3(1.0,1.0,1.0);
    vec3 d = vec3(0.263,0.416,0.577);
    
    return a + b*cos( 6.28318*(c*t+d));
}

void main()
{

    vec2 uv = gl_FragCoord.xy / iResolution.xy * 2.0 - 1.0;
    uv.x *= iResolution.x/iResolution.y;
    vec3 finalCol = vec3(0.0);
    
    vec2 uv0 = uv;
    
    for (float i = 0.0; i < 5.40; i++) {
        uv *= 1.7;
        uv = fract(uv);
        uv -= 0.5;
        
        float d = length(uv) * exp(-length(uv0));
        
        vec3 col = palette(length(uv0) + i*.4 + iTime);
        
        d = sin(d*8.0 + iTime*0.4)/8.0;
        d = abs(d);
        //d = smoothstep(0.0, 0.2, d);
        d = 0.01/d;
        finalCol += col * d;
    }


    fragColor = vec4(finalCol*0.5,1.0);
}
