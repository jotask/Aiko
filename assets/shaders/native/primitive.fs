#version 330 core

struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

struct Light
{
    int type;               // Type of the light (0: Directional, 1: Point, etc.)
 
    vec3 position;
    vec3 direction;
    vec3 color;
    float intensity;
  
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;  

};

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

#define NR_POINT_LIGHTS 4

uniform vec4 u_color;
uniform float u_border;
uniform float u_border_thickness;

uniform Light lights[NR_POINT_LIGHTS];
uniform int numLights;

uniform float ambientStrength;
uniform vec3 ambientColor;
uniform vec3 camPos;
uniform Material material;

vec3 CalcDirLight(vec3 direction, vec3 ambient, vec3 diffuse, vec3 specular, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 a = ambient * material.ambient;
    vec3 d = diffuse * diff * material.diffuse;
    vec3 s = specular * spec * material.specular;
    return (a + d + s);
}

// calculates the color when using a point light.
vec3 CalcPointLight(vec3 position, float constant, float linear, float quadratic, vec3 ambient, vec3 diffuse, vec3 specular, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(position - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
    // combine results
    vec3 a = ambient * material.ambient;
    vec3 d = diffuse * diff * material.diffuse;
    vec3 s = specular * spec * material.specular;
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    return (a + d + s);
}

// calculates the color when using a spot light.
vec3 CalcSpotLight(vec3 position, vec3 direction, float cutOff, float outerCutOff, float constant, float linear, float quadratic, vec3 ambient, vec3 diffuse, vec3 specular, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(position - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-direction)); 
    float epsilon = cutOff - outerCutOff;
    float intensity = clamp((theta - outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 a = ambient * material.ambient;
    vec3 d = diffuse * diff * material.diffuse;
    vec3 s = specular * spec * material.specular;
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (a + d + s);
}

vec3 calculateLighting(vec3 fragPos, vec3 normal, vec3 viewDir)
{

    if(numLights == 0)
    {
        return vec3(1.0);
    }

    vec3 totalLight = vec3(0.0);

    for (int i = 0; i < numLights; ++i)
    {
        Light light = lights[i];
        
        if (lights[i].type == 0)
        {
            totalLight += CalcDirLight(light.direction, light.ambient, light.diffuse, light.specular, normal, viewDir);
        }   
        else if (lights[i].type == 1)
        { 
            totalLight += CalcPointLight(light.position, light.constant, light.linear, light.quadratic, light.ambient, light.diffuse, light.specular, normal, fragPos, viewDir);
        }
        else if (lights[i].type == 2)
        { 
            totalLight += CalcSpotLight(light.position, light.direction, light.cutOff, light.outerCutOff, light.constant, light.linear, light.quadratic, light.ambient, light.diffuse, light.specular, normal, fragPos, viewDir);
        }
    }

    return totalLight;
}

void main()
{

    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camPos - FragPos);

    vec3 ambientLight = material.ambient * ambientColor;
    vec3 lighting = calculateLighting(FragPos, norm, viewDir);

    vec3 finalColor = ambientLight * lighting;

    FragColor = vec4(finalColor, 1.0) * u_color;

}
