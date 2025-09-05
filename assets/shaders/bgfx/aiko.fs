#include "varying.def.sc"

uniform sampler2D texture1;
uniform sampler2D texture2;

void main()
{
    gl_FragColor = vec4(v_color0, 1.0);

    // Optional: mix textures
    // gl_FragColor = mix(texture2D(texture1, v_texcoord0),
    //                    texture2D(texture2, v_texcoord0), 0.2);
}
