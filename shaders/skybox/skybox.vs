#version 330 core

layout (location = 0) in vec3 vertexPos;

out vec3 TexCoords;

uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

void main()
{
    vec4 pos = projectionMatrix * viewMatrix * vec4(vertexPos, 1.0);
    gl_Position = pos.xyww;
    TexCoords = vertexPos;
}
