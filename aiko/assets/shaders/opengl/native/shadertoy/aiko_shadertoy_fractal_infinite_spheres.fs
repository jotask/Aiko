#version 330

#define MAXSTEPS 100
#define MINDIST .01

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

float DistanceEstimator(vec3 pos) {
    
    // translate
    pos = pos + 1. * vec3(0,-0.5*iTime,iTime);

    float d1 = distance(mod(pos, 2.), vec3(1,1,1))-.54321;
    
    return d1;
}

float trace(vec3 from, vec3 direction) {
	float totalDistance = 0.0;
	int steps;
	for (steps=0; steps < MAXSTEPS; steps++) {
		vec3 p = from + totalDistance * direction;
		float dist = DistanceEstimator(p);
		totalDistance += dist;
		if (dist < MINDIST) break;
	}
	return 1.0-float(steps)/float(MAXSTEPS);
}

void main()
{

    vec2 uv = (gl_FragCoord.xy - 0.5*iResolution.xy) / iResolution.y;
    
    vec3 camPos = vec3(0, 2, 0);
	vec3 camViewDir = normalize(vec3(uv.xy, 1));
    
	float dist = trace(camPos, camViewDir);
    
    fragColor = vec4(dist, dist, dist, 1.0);

}
