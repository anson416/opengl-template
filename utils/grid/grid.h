#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "camera/camera.h"
#include "shader/shader.h"

class Grid
{
public:
    ~Grid(void);
    void setupGrid(const char* vertexPath, const char* fragmentPath, GLfloat far);
    void sendGridsToOpenGL(void);
    void draw(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, Camera camera);

private:
    enum _Component {
        _GRID_ORIGIN,
        _GRID_X_AXIS,
        _GRID_Y_AXIS,
        _GRID_Z_AXIS,
        _GRID_SQUARE,
        _GRID_COUNT,
    };

    struct _GridInfo {
        GLuint vaoID;
        GLsizei vertexCount;
    };
    _GridInfo* _gridInfo;
    
    Shader _gridShader;
    GLfloat _far;

    void _sendGrid(GLuint gridID, GLuint* vboID, GLuint* eboID, const GLfloat* data, GLint dataSize, const GLuint* indices, GLint indexSize);
};
