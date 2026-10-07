#version 330

const float E = 0.001;
const float FAR = 150.0;
const int STEPS = 64;

vec3 glow = vec3(0.0);

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

float sphere(vec3 p, float r){
    return length(p)-r;
}

void rot(inout vec2 p, float a){
    p = cos(a)*p + sin(a)* vec2(p.y, -p.x);
}

float scene(vec3 p){
    vec3 pp = p;

    float safe = sphere(pp, 23.0);

    float center = sphere(pp, 19.0);

    for(int i = 0; i < 7; ++i){
        pp = abs(pp)-vec3(0., 5., 1.0);
        rot(pp.xy, iTime*0.1);
        pp = abs(pp)-vec3(0., 0.7, 7.0);
        rot(pp.xy, iTime*0.05);
        pp = abs(pp)-vec3(0.0, 7.0, 0.5);
        rot(pp.zy, iTime*0.01);
    }

    float caleidos = sphere(pp, 7.);

    glow += vec3(0.5) * 0.01 / (abs(caleidos) + 0.01);
    glow += vec3(0.5) * 0.001 /(abs(center) + 0.01);

    caleidos = max(caleidos, 0.9);

    return min(center, max(caleidos, -safe));
}

float march(vec3 ro, vec3 rd){
    float t = E;
    vec3 p = ro;
    for(int i = 0; i < STEPS; ++i){
        float d = scene(p);
        t += d;
        p = ro + rd * t;

        if(d < E || t > FAR){
          break;
        }
    }
    return t;
}

void main()
{

    // Normalized pixel coordinates (from 0 to 1)
    vec2 uv = gl_FragCoord.xy/iResolution.xy;
    vec2 q = -1.0 + 2.0*uv;
    q.x *= iResolution.x/iResolution.y;

    vec3 ro = vec3(0.0, 0.25, 100.0);
    vec3 rt = vec3(0.0, 0.0, -1.0);

    vec3 z = normalize(rt-ro);
    vec3 x = normalize(cross(z, vec3(0.0, 1.0, 0.0)));
    vec3 y = normalize(cross(x, z));

    vec3 rd = normalize(mat3(x,y,z) * vec3(q, 1.0/radians(90.0)));

    float t = march(ro, rd);
    vec3 p = ro + rd * t;

    vec3 col = vec3(0.0, 0.0, 0.01);
    if(t < FAR){
        col = vec3(0.1, 0.5, 1.0)*100.0*(1.0/t);
    }

    col += glow;
    col = 1.0 - col;
    
    //In the original shader there is heavy motion blurr
    //vec3 prev = texture(texPreviousFrame, uv).rgb;
    //col = mix(col, prev, 0.9);

    col = smoothstep(-0.2, 1.2, col);
  
    // Output to screen
    fragColor = vec4(col,1.0);

}
