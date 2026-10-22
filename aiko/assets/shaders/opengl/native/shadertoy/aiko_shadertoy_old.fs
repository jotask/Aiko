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

float r(float n)
{
 	return fract(abs(sin(n*55.753)*367.34));   
}
float r(vec2 n)
{
    return r(dot(n,vec2(2.46,-1.21)));
}
float cycle(float n)
{
 	return cos(fract(n)*2.0*3.141592653)*0.5+0.5;
}

void main()
{
    float a = (radians(60.0));
    float zoom = 96.0;
	vec2 c = (gl_FragCoord.xy+vec2(iTime*zoom,0.0))*vec2(sin(a),1.0);
    c = ((c+vec2(c.y,0.0)*cos(a))/zoom)+vec2(floor(4.*(c.x-c.y*cos(a))/zoom),0.0);
    float n = cycle(r(floor(c*4.0))*0.2+r(floor(c*2.0))*0.3+r(floor(c))*0.5+iTime*0.125);
	fragColor = vec4(n*2.0,pow(n,2.0),0.0,1.0);
}
