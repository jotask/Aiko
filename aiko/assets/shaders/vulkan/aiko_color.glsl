
#ifndef AIKO_COLOR_GLSL
#define AIKO_COLOR_GLSL

// Convert normalized sRGB RGB values to linear-light RGB.
// Alpha and non-color data must not be converted.
vec3 aikoSrgbToLinear(vec3 encoded)
{
    vec3 srgb = clamp(encoded, vec3(0.0), vec3(1.0));

    vec3 lower = srgb / 12.92;
    vec3 upper = pow(
        (srgb + vec3(0.055)) / 1.055,
        vec3(2.4)
    );

    return mix(lower, upper, greaterThan(srgb, vec3(0.04045)) );
}

#endif

