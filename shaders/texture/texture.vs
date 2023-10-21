#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 vertexUV;
layout (location = 2) in vec3 normal;

out vec3 vertexPositionWorld;
out vec3 normalWorld;
out vec2 UV;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec4 newPosition = modelMatrix * vec4(position, 1.0f);
    gl_Position = projectionMatrix * viewMatrix * newPosition;
    
    vertexPositionWorld = newPosition.xyz;
    normalWorld = normalize((modelMatrix * vec4(normal, 0.0f)).xyz);
    UV = vertexUV;
}
