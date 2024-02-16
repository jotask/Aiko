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

#define PI 3.14159365
#define TAU 6.28318531

const int colorsPerGradient = 5;

//---------------------------------------------------------------------------------
//--------------------------------Color Functions----------------------------------
//------------------by nmz: https://www.shadertoy.com/view/XddGRN------------------

//-----------------HSV-----------------

//HSV functions from iq (https://www.shadertoy.com/view/MsS3Wc)
#ifdef SMOOTH_HSV
vec3 hsv2rgb( in vec3 c )
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

    rgb = rgb*rgb*(3.0-2.0*rgb); // cubic smoothing

    return c.z * mix( vec3(1.0), rgb, c.y);
}
#else
vec3 hsv2rgb(in vec3 c)
{
    vec3 rgb = clamp( abs(mod(c.x*6.0+vec3(0.0,4.0,2.0),6.0)-3.0)-1.0, 0.0, 1.0 );

    return c.z * mix( vec3(1.0), rgb, c.y);
}
#endif

//From Sam Hocevar: http://lolengine.net/blog/2013/07/27/rgb-to-hsv-in-glsl
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

//Linear interpolation between two colors in normalized (0..1) HSV space
vec3 lerpHSV(in vec3 a, in vec3 b, in float x)
{
    float hue = (mod(mod((b.x-a.x), 1.) + 1.5, 1.)-0.5)*x + a.x;
    return vec3(hue, mix(a.yz, b.yz, x));
}

//---------------Improved RGB--------------

/*
The idea behind this function is to avoid the low saturation area in the
rgb color space. This is done by getting the direction to that diagonal
and displacing the interpolated	color by it's inverse while scaling it
by saturation error and desired lightness.

I find it behaves very well under most circumstances, the only instance
where it doesn't behave ideally is when the hues are very close	to 180
degrees apart, since the method I am using to find the displacement vector
does not compensate for non-curving motion. I tried a few things to
circumvent this problem but none were cheap and effective enough..
*/

//Changes the strength of the displacement
#define DSP_STR 1.5

//Optimizaton for getting the saturation (HSV Type) of a rgb color
#if 0
float getsat(vec3 c)
{
    c.gb = vec2(max(c.g, c.b), min(c.g, c.b));
    c.rg = vec2(max(c.r, c.g), min(c.r, c.g));
    return (c.r - min(c.g, c.b)) / (c.r + 1e-7);
}
#else
//Further optimization for getting the saturation
float getsat(vec3 c)
{
    float mi = min(min(c.x, c.y), c.z);
    float ma = max(max(c.x, c.y), c.z);
    return (ma - mi)/(ma+ 1e-7);
}
#endif

//Improved rgb lerp
vec3 iLerp(in vec3 a, in vec3 b, in float x)
{
    //Interpolated base color (with singularity fix)
    vec3 ic = mix(a, b, x) + vec3(1e-6,0.,0.);
    //Saturation difference from ideal scenario
    float sd = abs(getsat(ic) - mix(getsat(a), getsat(b), x));
    //Displacement direction
    vec3 dir = normalize(vec3(2.*ic.x - ic.y - ic.z, 2.*ic.y - ic.x - ic.z, 2.*ic.z - ic.y - ic.x));
    //Simple Lighntess
    float lgt = dot(vec3(1.0), ic);
    //Extra scaling factor for the displacement
    float ff = dot(dir, normalize(ic));
    //Displace the color
    ic += DSP_STR*dir*sd*ff*lgt;
    return clamp(ic,0.,1.);
}

vec3 hsb2rgb(in vec3 hsb)
{
    vec3 rgb = clamp(abs(mod(hsb.x*6.0+vec3(0.0, 4.0, 2.0), 6.0)-3.0)-1.0, 0.0, 1.0);
    rgb = rgb*rgb*(3.0-2.0*rgb);
    return hsb.z * mix(vec3(1.0), rgb, hsb.y);
}


//--------------------------------------GRADIENT-----------------------------------
//--------------------------------color point array logic--------------------------
//-------------------------------------by Krabcode---------------------------------

struct colorPoint
{
    float pos;
    vec4 val;
};

colorPoint emptyColorPoint()
{
    return colorPoint(1., vec4(1.,0.,0.,1.));
}

float map(float value, float start1, float stop1, float start2, float stop2)
{
    return start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
}

float norm(float value, float start, float stop)
{
    return map(value, start, stop, 0., 1.);
}

vec4 lerpByBlendType(vec4 colorA, vec4 colorB, float amt, int blendType)
{
    float mixedAlpha = mix(colorA.a, colorB.a, amt);
    if(blendType == 0){ // normal lerp
        return mix(colorA, colorB, amt);
    }
    if(blendType == 1){ // normal lerp with improved saturation preservation
        return vec4(iLerp(colorA.rgb, colorB.rgb, amt), mixedAlpha);
    }
    if(blendType == 2){ // lerp between hues
        return vec4(hsv2rgb(lerpHSV(rgb2hsv(colorA.rgb), rgb2hsv(colorB.rgb), smoothstep(0.0, 1.0, amt))), mixedAlpha);
    }
    return vec4(0,0,0,1);
}

int findClosestLeftNeighbourIndex(float pos, colorPoint[colorsPerGradient] gradient)
{
    for(int i = 0; i < 100; i++){
        if(pos >= gradient[i].pos && pos <= gradient[i+1].pos){
            return i;
        }
        if(i >= gradient.length()){
            return 0;
        }
    }
    return 0;
}

vec4 gradientColorAt(float normalizedPos, colorPoint[colorsPerGradient] gradient, int blendType)
{
    float pos = clamp(normalizedPos, 0., 1.);
    int leftIndex = findClosestLeftNeighbourIndex(pos, gradient);
    int rightIndex = leftIndex + 1;
    colorPoint A = gradient[leftIndex];    
    colorPoint B = gradient[rightIndex];
    float normalizedPosBetweenNeighbours = norm(pos, A.pos, B.pos);
    vec4 mixedColor = lerpByBlendType(A.val, B.val, normalizedPosBetweenNeighbours, blendType);
    return mixedColor;
}

// hexToRgb from here: https://stackoverflow.com/questions/22895237/hexadecimal-to-rgb-values-in-webgl-shader
vec3 hexToRgb(int color)
{
    float rValue = float(color / 256 / 256);
    float gValue = float(color / 256 - int(rValue * 256.0));
    float bValue = float(color - int(rValue * 256.0 * 256.0) - int(gValue * 256.0));
    return vec3(rValue / 255.0, gValue / 255.0, bValue / 255.0);
}

/** example gradient

	colorPoint[colorsPerGradient] gradient = colorPoint[](
        colorPoint(0.,  vec4(hexToRgb(0x2a3d66), 1.)),
        colorPoint(0.25,vec4(hexToRgb(0x5d54a4), 1.)),
        colorPoint(0.5, vec4(hexToRgb(0x9d65c9), 1.)),
        colorPoint(1.,vec4(hexToRgb(0xd789d7), 1.))
    );

*/

float time;
vec2 m;

// IQ noise and domain warp
float hash(float n)
{
    return fract(sin(n)*43758.5453);
}

float iqNoise(vec3 x){
    vec3 p = floor(x);
    vec3 f = fract(x);
    f  = f*f*(3.0-2.0*f);
    float n = p.x + p.y*57.0 + 113.0*p.z;
    return mix(mix(mix(hash(n+0.0), hash(n+1.0), f.x),
    mix(hash(n+57.0), hash(n+58.0), f.x), f.y),
    mix(mix(hash(n+113.0), hash(n+114.0), f.x),
    mix(hash(n+170.0), hash(n+171.0), f.x), f.y), f.z);
}


float fbm(vec2 p){
    float sum = 0.;
    float freq = 1.;
    float amp = 0.5;
    for (int i = 0; i < 1+int(floor(m.x*4.)); i++){
        sum += amp*(1.-2.*iqNoise(vec3(p*freq, 0.)));
        freq *= 2.0;
        amp *= .5;
    }
    return sum;
}

float pattern( in vec2 p, out vec2 q, out vec2 r )
{
    q.x = fbm( p + vec2(0.0+time,0.0-time) );
    q.y = fbm( p + vec2(5.2-time,1.3) );

    r.x = fbm( p + 4.0*q + vec2(1.7,9.2) );
    r.y = fbm( p + 4.0*q + vec2(8.3,2.8) );

    return fbm( p + 4.0*r );
}

float render(vec2 uv){
 	float a = cos(8.*atan(uv.y, uv.x));
    float d = length(uv*(2.5+m.y*25.))-time*.05;
    vec2 p = vec2(d, a);
    vec2 q = vec2(0.);
    vec2 r = vec2(0.);
    float n = .25+.75*pattern(p, q, r);
    n *= length(q*2.);
    n += r.x;
    n -= r.y;
    return n;
}

float renderAA(vec2 uv){
    float pixelThird = (1./iResolution.x) / 3.0;
    vec2 aa = vec2(-pixelThird, pixelThird);
    float c1 = render(uv+aa.xx);
    float c2 = render(uv+aa.xy);
    float c3 = render(uv+aa.yx);
    float c4 = render(uv+aa.yy);
    return (c1+c2+c3+c4) / 4.;
}

void main()
{
    // https://colorhunt.co/palette/179481
	colorPoint[colorsPerGradient] gradient = colorPoint[](
        colorPoint(0., vec4(hexToRgb(0x522d5b)*.5, 1)),
        colorPoint(0.25,  vec4(hexToRgb(0x522d5b), 1.)),
        colorPoint(0.5,vec4(hexToRgb(0xd7385e), 1.)),
        colorPoint(0.75, vec4(hexToRgb(0xfb7b6b), 1.)),
        colorPoint(1.0,vec4(hexToRgb(0xe7d39f), 1.))
    );
    time = float(iFrame)*0.001;
    m = iMouse.xy / iResolution.xy;
    if(length(m) < 0.01){
      m = vec2(.1);   
    }
    vec2 uv = (gl_FragCoord.xy - .5*iResolution.xy) / iResolution.y;
	float n = renderAA(uv);
    fragColor = gradientColorAt(n, gradient, 0);
}
