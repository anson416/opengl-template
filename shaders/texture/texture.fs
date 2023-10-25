/*
References: https://learnopengl.com/Lighting/Light-casters
            https://learnopengl.com/Lighting/Multiple-lights
*/

#version 330 core

/* ----- Modify macros here ----- */
#define N_POINT_LIGHTS 1
#define N_DIR_LIGHTS 6
#define N_SPOT_LIGHTS 1
/* ------------------------------ */

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 diffuseK;
    vec3 specularK;
    vec3 intensity;
};

struct Attenuation {
    float a;
    float b;
    float c;
};

struct PointLight {
    Light light;
    vec3 pos;
    Attenuation attenuation;
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
    Attenuation attenuation;
};

in vec3 vertexPosWorld;
in vec3 normalWorld;
in vec2 UV;

out vec4 FragColor;

uniform vec3 eyePosWorld;
uniform Material material;
uniform bool useBlinn;
uniform vec3 emissionK;
uniform vec3 ambientK;
uniform PointLight pointLights[N_POINT_LIGHTS];
uniform DirLight dirLights[N_DIR_LIGHTS];
// uniform SpotLight spotLights[N_SPOT_LIGHTS];

vec3 calPointLight(PointLight light, vec3 normal, vec3 vertexPos, vec3 viewDir);
vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir);
vec3 calSpotLight(SpotLight light, vec3 normal, vec3 vertexPos, vec3 viewDir);

void main()
{
    int i;
    vec3 viewDir = normalize(eyePosWorld - vertexPosWorld);
    
    vec3 result = emissionK + ambientK * texture(material.diffuse, UV).rgb;
    for (i = 0; i < N_POINT_LIGHTS; i++)
        result += calPointLight(pointLights[i], normalWorld, vertexPosWorld, viewDir);
    for (i = 0; i < N_DIR_LIGHTS; i++)
        result += calDirLight(dirLights[i], normalWorld, viewDir);
    // for (i = 0; i < N_SPOT_LIGHTS; i++)
    //     result += calSpotLight(spotLights[i], normalWorld, vertexPosWorld, viewDir);
    
    FragColor = vec4(result, 1.0f);
}

vec3 calPointLight(PointLight light, vec3 normal, vec3 vertexPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - vertexPos);
    
    /* Diffuse reflection */
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = light.light.diffuseK * texture(material.diffuse, UV).rgb * diff;
    
    /* Specular reflection */
    float spec = 0.0f;
    if (useBlinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);
    }
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    }
    vec3 specular = light.light.specularK * texture(material.specular, UV).rgb * spec;
    
    /* Attenuate */
    float dist = length(light.pos - vertexPos);
    float attenuation = min(1.0f / (light.attenuation.a + light.attenuation.b * dist + light.attenuation.c * pow(dist, 2)), 1.0f);
    
    return attenuation * light.light.intensity * (diffuse + specular);
}

vec3 calDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.dir);
    
    /* Diffuse reflection */
    float diff = max(dot(lightDir, normal), 0.0f);
    vec3 diffuse = light.light.diffuseK * texture(material.diffuse, UV).rgb * diff;
    
    /* Specular reflection */
    float spec = 0.0f;
    if (useBlinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);
    }
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    }
    vec3 specular = light.light.specularK * texture(material.specular, UV).rgb * spec;
    
    return light.light.intensity * (diffuse + specular);
}

vec3 calSpotLight(SpotLight light, vec3 normal, vec3 vertexPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.pos - vertexPos);
    
    /* Diffuse */
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = light.light.diffuseK * texture(material.diffuse, UV).rgb * diff;
    
    /* Specular reflection */
    float spec = 0.0f;
    if (useBlinn) {
        vec3 halfwayDir = normalize(lightDir + viewDir);
        spec = pow(max(dot(normal, halfwayDir), 0.0f), material.shininess);
    }
    else {
        vec3 reflectDir = reflect(-lightDir, normal);
        spec = pow(max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    }
    vec3 specular = light.light.specularK * texture(material.specular, UV).rgb * spec;
    
    /* Attenuate */
    float dist = length(light.pos - vertexPos);
    float attenuation = min(1.0f / (light.attenuation.a + light.attenuation.b * dist + light.attenuation.c * pow(dist, 2)), 1.0f);
    
    /* Spotlight intensity */
    float theta = dot(lightDir, normalize(-light.dir));
    float epsilon = light.cutOff - light.outerCutOff;
    float cutoff_intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    return attenuation * light.light.intensity * cutoff_intensity * (diffuse + specular);
}
