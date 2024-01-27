#version 330

#define FLT_MAX 3.402823466e+38
#define FLT_MIN 1.175494351e-38

#define MAX_NUM_TOTAL_PARTICLES 100

in vec2 fragTexCoord;
in vec4 fragColor;

out vec4 finalColor;

uniform vec2 particles[MAX_NUM_TOTAL_PARTICLES];

vec3 Hsv2rgb(float hue, float saturation, float value)
{
    vec4 K = vec4(1.0f, 2.0f/3.0f, 1.0f/3.0f, 3.0f);
    vec3 p = abs(fract(vec3(hue,hue,hue) + K.xyz)*6.0f - K.www);
    return value * mix(K.xxx, clamp(p - K.xxx, 0.0f, 1.0f), saturation);
}

float map(float input, float input_start, float input_end, float output_start, float output_end)
{
    if (input_start == input_end) return output_start;
    return output_start + ((output_end - output_start) * (input - input_start)) / (input_end - input_start);
}

void main()
{

    vec2 pos = gl_FragCoord.xy;

    float minDistance = FLT_MAX;
    float maxDistance = FLT_MIN;

    for (int i = 0; i < MAX_NUM_TOTAL_PARTICLES; i++)
    {
        float distance = distance(pos, particles[i]);
        minDistance = min(minDistance, distance);
        maxDistance = max(maxDistance, distance);
    }

    float hue = map( minDistance, 0.0f, maxDistance, 0.0f, 1.0f );
    float saturation = 1.0f;
    float value = 0.75f;

    // pixels[y * screen.x + x] = ColorFromHSV(hue, saturation, value);

    finalColor = vec4(Hsv2rgb(hue, saturation, value), 1.0f);

    }
