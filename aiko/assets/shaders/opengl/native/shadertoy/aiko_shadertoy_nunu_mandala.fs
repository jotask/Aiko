#version 330

const float PI = 3.141592653589793;

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

mat2 rotate2D(float t){
    float c = cos(t);
    float s = sin(t);
    return mat2(c, s, -s, c);
}


float atan2(float y, float x){
    return x == 0.0 ? sign(y) * 0.5 * PI : atan(y, x);
}


vec2 pmod(vec2 p, float n){
    float a = mod(atan2(p.y, p.x), 2.0*PI/n)-PI/n;
    return length(p)*vec2(cos(a), sin(a));
}

void main()
{

    // 制御変数：control variable
    float scale = 12.0;  // 表示範囲：Display range
    float k = 25.0;  // イテレーションの回数：iteration number
    float a0 = 7.0;  // 回転対称性の数：rotational symmetry
    float a1 = 91.0;  // a0 よりも大きい数を入れる：a0 < a1
    float fl = 2.0;  // box fold の左側：box fold left
    float fr = 3.0;  // box fold の右側：box fold right
    float cp = 12.0;  // 点滅の調整：flashing
    float ct = 1.0;  // 点滅周期：blinking period
    
    // 処理：process
    vec2 p = scale * (2.0*gl_FragCoord.xy - iResolution.xy) / min(iResolution.x, iResolution.y);
    p = -p.yx;
    float j;  // box fold の範囲をイテレーションの中で少しずつ変える。
    for (float i=0.0; i<k; i++){
        j = i/k;
        p = pmod(p, a0);
        p.y = abs(p.y);
        p = 2.0 * clamp(p, fl-j, fr+j) - p;
        p = pmod(p, a1);
    }
    p = 0.5 + 0.5*cos(2.0*PI*(cp*p+ct*iTime));
    fragColor = p.xyyy;
}
