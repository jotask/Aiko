#version 330

vec3 COLOR_OUTLINE = vec3(0.);
vec3 COLOR_1 = vec3(.807, .517, .678);
vec3 COLOR_2 = vec3(0.807, 0.588, 0.650);
vec3 COLOR_GREEN = vec3(0.698, 0.792, 0.568);


#define S(a,b,c) smoothstep(a,b,c)

const float pi2 = 6.2831;
const float pi = 3.1415;

#define S(a,b,c) smoothstep(a,b,c)

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

mat2 Rot(float a) { 
    float s = sin(a), c = cos(a);
    return mat2(c, -s, s, c);
}


vec4 Circle(vec2 uv, vec3 color, float r, float blur) { 
    float d = length(uv);
    float a = S(r, r - blur, length(uv));
    return vec4(color, a);    
}


float RectByWidth(vec2 uv, float wb, float wt, float h, float edge) {
    
    float width = (wt - wb) * (h - uv.y)/h;
    float x1 = (width * .5);
    float x2 = -width * .5; //wb at y = 0, and wt at y = wt        
    float y1 = h;
    float y2 = 0.;
        
    float x = S(x1 - edge, x1, uv.x) * S(x2, x2-edge, uv.x);
    float y = S(y1, y1-edge, uv.y) * S(y2, y2+edge, uv.y);
    
    return x*y;
}

float Rect(vec2 uv, float x1, float y1, float w, float h, float edge) {
    
    float x2 = x1 + w;
    float y2 = y1 - h;
    
    float x = S(x1 - edge, x1, uv.x) * S(x2, x2-edge, uv.x);
    float y = S(y1, y1-edge, uv.y) * S(y2, y2+edge, uv.y);
    
    return x*y;
}

vec4 Flower(vec2 uv, vec2 pos, float angle, vec3 baseColor, float yScale) {             
    uv -= pos;
    uv *= Rot(angle);    
    vec2 st = vec2(atan(uv.x, uv.y), length(uv));
    uv = vec2(st.x / 6.2831+.5, st.y); //x will be 0 to 1 from bottom to bottom going clockwise 
                                       //y will be the distance from the center 
    uv.y *= yScale;
    
    float x = uv.x * 5.; //you go from 0 to 1
    
    float id = floor(x);
    float m = min(fract(x), fract(1.-x)); //assuming going from 0 to 1 it will give it a num
                                          //going from 0 to .5 and then back to .0
                                          
    float s = max(((m+.3) - uv.y), 0.); // * (id == 2. ? 1. : 0.);    
    float c = smoothstep(.0, .01, s);
    
    return vec4(baseColor, c); //played with this to get a leaf I like
}


vec4 DrawCenterCircle(vec2 uv) { 
    vec4 color = vec4(0.);
    vec4 center_bg = Circle(uv, vec3(0., 0., 0.), .22, .004);
    vec4 center = Circle(uv, COLOR_1, .2, .004);
    
    color = mix(color, center_bg, center_bg.a);
    color = mix(color, center, center.a);
    
    return color;
}

vec4 DrawRects(vec2 uv, float count, vec3 baseColor, float dist, float startAngle) { 
    vec4 col = vec4(0.);    
    
    for(float d = 0.; d < pi2; d+= pi2/count) {        
        vec2 pos = uv;

        pos -= vec2(sin(startAngle + d), cos(startAngle+ d)) * dist;
        pos = pos * Rot(1./4. * pi + d + startAngle + 0.02);
        
        vec4 rect = vec4(baseColor, Rect(pos, -.2, .2, .3, .3, .01));
        col = mix(col, rect, rect.a);
    }
    
    return col;
}

vec4 DrawTriangles(vec2 uv, float count, vec3 baseColor, float dist, float startAngle) { 
    dist = 1.21;
    startAngle += 0.26;
//    float r = RectByWidth(uv, 1., .0, 1., 0.01);
//    col = mix(col, vec4(.5, .5, .5, r), r); 
    vec4 col = vec4(0.);
    for(float d = 0.; d < pi2; d+= pi2/count) {        
        vec2 pos = uv;

        pos -= vec2(sin(startAngle + d), cos(startAngle+ d)) * dist;
        pos = pos * Rot(d - 0.405);
        
        vec4 rect = vec4(baseColor, RectByWidth(pos, .28, .0, .3 + sin(iTime)*.03, .02));
        col = mix(col, rect, rect.a);
    }
    
    return col;
}

vec4 DrawCircles(vec2 uv, float count, vec3 baseColor, float dist, float radius, float startAngle) { 
    vec4 col = vec4(0.);    
    
    for(float d = 0.; d < pi2; d+= pi2/count) {        
        vec2 pos = uv;

        pos -= vec2(sin(startAngle + d), cos(startAngle+ d)) * dist;
        
        vec4 rect = Circle(pos, baseColor, radius, .01);
        col = mix(col, rect, rect.a);
    }
    
    return col;
}

vec4 DrawHalfCircles(vec2 uv, float count, vec3 baseColor, float dist, float bigR, float lilR, float startAngle) { 
    vec4 col = vec4(0.);    
    
    for(float d = 0.; d < pi2; d+= pi2/count) {        
        vec2 pos = uv;

        pos -= vec2(sin(startAngle + d), cos(startAngle+ d)) * dist;
        pos *= Rot(d + 1./4. * pi  + sin(iTime)*2.);
        vec4 c1 = Circle(pos, baseColor, bigR, .01);
        vec4 c2 = Circle(pos, baseColor, lilR, .01);
        
        float rect = Rect(pos, -bigR, 0., bigR * 2., bigR, .005);
        
        col = mix(col, c1, c1 * (1. - c2.a) * (1. - rect));
    }
    
    return col;
}


vec4 DrawLeaves(vec2 uv, float leafCount, vec3 leafColor, float scale, float yScale) { 
    vec4 col = vec4(0.);    
    
    float leafDistanceScale = 0.;
    float d = 0.;
//    for(float d = 0.; d < pi2; d+= pi2/1.) {        
        vec2 pos = uv;
        
        vec4 flower = Flower(pos * scale, vec2(sin(d), cos(d)) * leafDistanceScale, d, leafColor, yScale);
        vec4 flower_outline = Flower(pos * (scale * .9), vec2(sin(d), cos(d)) * leafDistanceScale, d, vec3(0., 0., 0), yScale);

        col = mix(col, flower_outline, flower_outline.a);
        col = mix(col, flower, flower.a);
//    }
    
    return col;
}

float sin01(float time) { 
    return sin(time)*.5+.5;
}

void main()
{

    float time = iTime;
    vec2 uv = (gl_FragCoord.xy-.5*iResolution.xy)/iResolution.y;
    uv *= 3.3;    
    
    vec4 col = vec4(1.);    
    
    
    vec4 half_circles_1 = DrawHalfCircles(uv , 5., vec3(.6, .3, .3), 1.25 +  + sin01(time - pi)*.15, .25, .17, 0.63);
    
    vec4 triangles_1 = DrawTriangles(uv, 5., COLOR_GREEN, 1., 0.);
    
    vec4 circles_1 = DrawCircles(uv , 5., vec3(.5, .2, .2), 1.25 + sin01(time - pi)*.15, .14, 0.63 );
    
    vec4 diamonds_1 = DrawRects(uv, 5., vec3(0.), .85 + sin01(time - pi)*.15, 0.369);
    vec4 diamonds_2 = DrawRects(uv, 5., vec3(0.), .85 + sin01(time - pi)*.15, 0.369 + 0.519);
    
    vec4 leaf_1 = DrawLeaves(uv , 7., COLOR_2, 1., 1. + + sin01(time)*.2);
    vec4 leaf_2 = DrawLeaves(uv * Rot(2./4. * pi2) , 10., COLOR_GREEN, .8, 1. + sin01(time)*.1);
    vec4 leaf_3 = DrawLeaves(uv * Rot(4./4. * pi2) , 10., COLOR_GREEN, .7, 1. + + sin01(time)*.05);


    col = mix(col, half_circles_1, half_circles_1.a);
    
    float a = S(1., .8, sin01(time - pi));
    col = mix(col, triangles_1, triangles_1.a - a);
    col = mix(col, circles_1, circles_1.a);
//    col = mix(col, circles_2, circles_2.a);
    
    col = mix(col, diamonds_1, diamonds_1.a);
    col = mix(col, diamonds_1, diamonds_2.a);
    
    col = mix(col, leaf_3, leaf_3.a);
    col = mix(col, leaf_2, leaf_2.a);
    col = mix(col, leaf_1, leaf_1.a);
    
    vec4 center = DrawCenterCircle(uv);
    col = mix(col, center, center.a);

    fragColor = vec4(col);
}
