$input v_texcoord0, v_color0, v_normal, v_worldPos

#include "bgfx_shader.sh"

uniform vec4 u_baseColor;          // rgba
uniform vec4 u_flags;              // x=useTexture, y=useVertexColor, z=lit

uniform vec4 u_cameraPos;
uniform vec4 u_ambientColor;        
uniform vec4 u_ambientIntensity; 

uniform vec4 u_lightCount;                  // count

#define MAX_LIGHTS 8
uniform vec4 u_lightType[MAX_LIGHTS];      // x = type (0/1/2)
uniform vec4 u_lightPosRange[MAX_LIGHTS];  // xyz = pos, w = range
uniform vec4 u_lightDir[MAX_LIGHTS];       // xyz = dir (normalized)
uniform vec4 u_lightColorInt[MAX_LIGHTS];  // rgb = color, w = intensity
uniform vec4 u_lightSpotCos[MAX_LIGHTS];   // x = innerCos, y = outerCos

SAMPLER2D(u_texture, 0);

void main()
{
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}