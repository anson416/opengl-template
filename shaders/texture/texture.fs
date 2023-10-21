/*
References: https://learnopengl.com/Lighting/Light-casters
            https://learnopengl.com/Lighting/Multiple-lights
*/

#version 330 core

/* ----- Modify macros here ----- */
#define N_POINT_LIGHTS 1
#define N_DIR_LIGHTS 1
#define N_SPOT_LIGHTS 1
/* ------------------------------ */

struct Light {
    vec3 diffuseK;
    vec3 specularK;
    vec3 intensity;
    int highlight;
};

struct PointLight {
    Light light;
    vec3 pos;
    float a;
    float b;
    float c;
};

struct DirLight {
    Light light;
    vec3 dir;
};

struct SpotLight {
    Light light;
    vec3 pos;
    vec3 dir;
    float cutOff;
    float outerCutOff;
    float a;
    float b;
    float c;
};

in vec3 vertexPositionWorld;
in vec3 normalWorld;
in vec2 UV;

out vec4 FragColor;

uniform vec3 eyePositionWorld;

uniform vec3 emissionK;
uniform vec3 ambientK;
uniform vec3 ambientIntensity;
uniform bool useBlinn;
uniform PointLight pointLights[N_POINT_LIGHTS];
uniform DirLight dirLights[N_DIR_LIGHTS];
uniform SpotLight spotLights[N_SPOT_LIGHTS];
uniform sampler2D myTextureSampler0;

vec3 calPointLight(PointLight light, vec3 normal, vec3 vertexPos, vec3 viewDir);
vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calSpotLight(SpotLight light, vec3 normal, vec3 vertexPos, vec3 viewDir);

void main()
{
    int i;
    vec3 viewDir = normalize(eyePositionWorld - vertexPositionWorld);
    
    vec3 finalLight = emissionK + ambientK * ambientIntensity;
    for (i = 0; i < N_POINT_LIGHTS; i++)
        finalLight += calPointLight(pointLights[i], normalWorld, vertexPositionWorld, viewDir);
    for (i = 0; i < N_DIR_LIGHTS; i++)
        finalLight += calDirLight(dirLights[i], normalWorld, viewDir);
    for (i = 0; i < N_SPOT_LIGHTS; i++)
        finalLight += calSpotLight(spotLights[i], normalWorld, vertexPositionWorld, viewDir);
    
    FragColor = vec4(texture(myTextureSampler0, UV).rgb * finalLight, 1.0f);
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 vertexPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - vertexPos);
    
    /* Diffuse */
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = light.light.diffuseK * diff;
    
    /* Specular */
    vec3 specDir = useBlinn ? normalize(lightDir + viewDir) : reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, specDir), 0.0f), light.light.highlight);
    vec3 specular = light.light.specularK * spec;
    
    /* Attenuate */
    float dist = length(light.pos - vertexPos);
    float attenuation = min(1.0f / (light.a + light.b * dist + light.c * pow(dist, 2)), 1.0f);
    
    return attenuation * light.light.intensity * (diffuse + specular);
}

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.dir);
    
    /* Diffuse */
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = light.light.diffuseK * diff;
    
    /* Specular */
    vec3 specDir = useBlinn ? normalize(lightDir + viewDir) : reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, specDir), 0.0f), light.light.highlight);
    vec3 specular = light.light.specularK * spec;
    
    return light.light.intensity * (diffuse + specular);
}

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 vertexPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - vertexPos);
    
    /* Diffuse */
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.light.diffuseK * diff;
    
    /* Specular */
    vec3 specDir = useBlinn ? normalize(lightDir + viewDir) : reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, specDir), 0.0f), light.light.highlight);
    vec3 specular = light.light.specularK * spec;
    
    /* Attenuate */
    float dist = length(light.pos - vertexPos);
    float attenuation = min(1.0f / (light.a + light.b * dist + light.c * pow(dist, 2)), 1.0f);
    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.dir));
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    return attenuation * light.light.intensity * intensity * (diffuse + specular);
}
