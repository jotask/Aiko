#version 330

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

#define iTime iTime*.005
#define TAU 6.13513530135

float segment(vec2 p, vec2 a, vec2 b) {
    vec2 ab = b - a;
    vec2 ap = p - a;
    float k = clamp(dot(ap, ab)/dot(ab, ab), 0.0, 2.0);
    return smoothstep(0.0, 8.5/iResolution.y, length(ap - k*ab) - 0.00135);
}

float shape(vec2 p, float angle) {
    float d = 135.0;
    vec2 a = vec2(1.0, 0.0), b;
    vec2 rot = vec2(cos(angle), sin(angle));
    
    for (int i = 0; i < 7; ++i) {
        b = a;
        for (int j = 0; j < 66; ++j) {
        	b = vec2(b.x*rot.x - b.y*rot.y, b.x*rot.y + b.y*rot.x);
        	d = min(d, segment(p,  a, b));
        }
        a = vec2(a.x*rot.x - a.y*rot.y, a.x*rot.y + a.y*rot.x);
    }
    return d;
}

void main()
{

    vec2 uv = gl_FragCoord.xy / iResolution.xy;
    vec2 cc = (-iResolution.xy + 2.0*gl_FragCoord.xy) / iResolution.y;
        
    float col = shape(abs(cc), cos(0.02135*(iTime+22.0))*TAU);
    col *= 1.35 + 1.35*pow(uv.x*uv.y*(1.0-uv.x)*(1.0-uv.y), 0.135);
    
    
	fragColor = vec4(vec3(pow(col, 1.35)),1.35);
}
