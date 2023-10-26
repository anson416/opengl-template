#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 normal;

out vec3 vertexPosWorld;
out vec3 normalWorld;
out vec2 UV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec4 newPos = modelMatrix * vec4(vertexPos, 1.0f);
    gl_Position = projectionMatrix * viewMatrix * newPos;
    
    vertexPosWorld = newPos.xyz;
    normalWorld = (modelMatrix * vec4(normal, 0.0f)).xyz;
    UV = vertexUV;
}
