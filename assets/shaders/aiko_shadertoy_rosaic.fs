#version 330

#define PI 3.1415926535897932384626433832795

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

    int rayCount = 12;
    vec3 color1 = vec3(1.,.9,0.);
    vec3 color2 = vec3(0.,0.4,.3);
    
    // center
    vec2 c = (gl_FragCoord.xy - vec2(iResolution) * .5) / iResolution.y;
    
    // cartesian to polar
	float angle = atan(c.y, c.x);
    float dist = length(c);
    
    // normalize angle
    angle /= (2.*PI);
    
    // fraction angle
    float mask = 1.;
    
    // radial gradient
    float distRound = (1. - ceil(dist*10.*.5+.25)*.1);
    float time2 = (fract(dist*10.*.5+.25) > 0.5 ? -iTime : iTime) * distRound; // cw/ccw alternatively
    float ngfract = fract(angle * round(10. / (distRound * distRound)) + time2 * .6);
    ngfract = abs(ngfract*2. - 1.);
    ngfract *= fract(dist*10.) > .5 ? -1. : 1.;
    mask -= ceil(dist*10. + .5 + ngfract*.5)*.1;
    
    //mask *= .8 + .2 * fract(dist*10.*.5+.25);
    
    // output
    fragColor = vec4(mix(vec3(.7, .0, .9)*.3,mix(color2, color1, mask),distRound),1.0);
}
