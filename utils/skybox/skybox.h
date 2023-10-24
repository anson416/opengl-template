#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "shader/shader.h"
#include "texture/texture.h"

#include <vector>

class Skybox
{
public:
    void setupSkybox(const char* vertexPath, const char* fragmentPath, const std::vector<std::string> texPaths);
    void draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix);

private:
    Shader _skyboxShader;
    Texture _skyboxTex;
    GLuint _skyboxVAO;
};
