#include "grid.h"

#include "glm/gtc/matrix_transform.hpp"

#include "camera/camera.h"
#include "misc/misc.h"

#include <iostream>

Grid::~Grid(void)
{
    delete[] _gridInfo;
}

void Grid::setupGrid(const char* vertexPath, const char* fragmentPath, GLfloat far)
{
    _gridInfo = new(std::nothrow) _GridInfo[_GRID_COUNT];
    if (!_gridInfo) {
        std::cerr << "ERR: Failed to allocate memory" << std::endl;
        exit(1);
    }
    _gridShader.setupShader(vertexPath, fragmentPath);
    _far = far;
}

void Grid::sendGridsToOpenGL(void)
{
    GLuint vboID, eboID;
    int i, j;

    /* Origin */
    const GLfloat origin[] = {
        0.0f, 0.0f, 0.0f, R(0), G(255), B(255),
    };
    const GLuint originIndices[] = {0};
    _sendGrid(_GRID_ORIGIN, &vboID, &eboID, origin, sizeof(origin), originIndices, sizeof(originIndices));
    
    /* x-axis */
    const GLfloat xAxis[] = {
         1.0f, 0.0f, 0.0f, R(255), G(0), B(0),
        -1.0f, 0.0f, 0.0f, R(255), G(0), B(0),
    };
    const GLuint xAxisIndices[] = {0, 1};
    _sendGrid(_GRID_X_AXIS, &vboID, &eboID, xAxis, sizeof(xAxis), xAxisIndices, sizeof(xAxisIndices));
    
    /* y-axis */
    const GLfloat yAxis[] = {
        0.0f,  1.0f, 0.0f, R(0), G(255), B(0),
        0.0f, -1.0f, 0.0f, R(0), G(255), B(0),
    };
    const GLuint yAxisIndices[] = {0, 1};
    _sendGrid(_GRID_Y_AXIS, &vboID, &eboID, yAxis, sizeof(yAxis), yAxisIndices, sizeof(yAxisIndices));
    
    /* z-axis */
    const GLfloat zAxis[] = {
        0.0f, 0.0f,  1.0f, R(0), G(0), B(255),
        0.0f, 0.0f, -1.0f, R(0), G(0), B(255),
    };
    const GLuint zAxisIndices[] = {0, 1};
    _sendGrid(_GRID_Z_AXIS, &vboID, &eboID, zAxis, sizeof(zAxis), zAxisIndices, sizeof(zAxisIndices));
    
    /* Squares */
    const GLint n_side_edges    = static_cast<GLint>(_far) << 1;
    const GLint n_side_vertices = n_side_edges + 1;
    GLint starting_idx, half;
    GLfloat squares[n_side_vertices * n_side_vertices * 6];
    for (i = 0; i <= n_side_edges; i++) {
        for (j = 0; j <= n_side_edges; j++) {
            starting_idx = (i * n_side_vertices + j) * 6;
            squares[starting_idx + 0] = static_cast<GLfloat>(i);
            squares[starting_idx + 1] = 0.0f;
            squares[starting_idx + 2] = static_cast<GLfloat>(j);
            squares[starting_idx + 3] = R(255);
            squares[starting_idx + 4] = G(255);
            squares[starting_idx + 5] = B(255);
        }
    }
    half = n_side_edges * n_side_vertices;
    GLuint squareIndices[half << 2];
    for (i = 0; i <= n_side_edges; i++) {
        for (j = 0; j < n_side_edges; j++) {
            starting_idx = (i * n_side_edges + j) << 1;
            squareIndices[starting_idx + 0] = j + i * n_side_vertices;
            squareIndices[starting_idx + 1] = j + i * n_side_vertices + 1;
        }
    }
    for (j = 0; j <= n_side_edges; j++) {
        for (i = 0; i < n_side_edges; i++) {
            starting_idx = (half + (j * n_side_edges + i)) << 1;
            squareIndices[starting_idx + 0] = j + i * n_side_vertices;
            squareIndices[starting_idx + 1] = j + (i + 1) * n_side_vertices;
        }
    }
    _sendGrid(_GRID_SQUARE, &vboID, &eboID, squares, sizeof(squares), squareIndices, sizeof(squareIndices));
}

void Grid::_sendGrid(GLuint gridID, GLuint* vboID, GLuint* eboID, const GLfloat* data, GLint dataSize, const GLuint* indices, GLint indexSize)
{
    glGenVertexArrays(1, &_gridInfo[gridID].vaoID);
    glBindVertexArray(_gridInfo[gridID].vaoID);
    
    glGenBuffers(1, vboID);
    glBindBuffer(GL_ARRAY_BUFFER, *vboID);
    glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
    
    glGenBuffers(1, eboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexSize, indices, GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
    
    _gridInfo[gridID].vertexCount = indexSize / sizeof(GLuint);
}

void Grid::drawGrids(glm::mat4 viewMatrix, glm::mat4 projectionMatrix, Camera camera)
{
    _gridShader.use();
    _gridShader.setMat4("viewMatrix", viewMatrix);
    _gridShader.setMat4("projectionMatrix", projectionMatrix);

    glm::mat4 modelMatrix;
    
    /* Origin */
    glBindVertexArray(_gridInfo[_GRID_ORIGIN].vaoID);
    modelMatrix = glm::mat4(1.0f);
    _gridShader.setMat4("modelMatrix", modelMatrix);
    glDrawElements(GL_POINTS, _gridInfo[_GRID_ORIGIN].vertexCount, GL_UNSIGNED_INT, 0);
    
    /* x-axis */
    glBindVertexArray(_gridInfo[_GRID_X_AXIS].vaoID);
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(glm::max(camera.getPos().x, _far), 1.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(_far, 1.0f, 1.0f));
    _gridShader.setMat4("modelMatrix", modelMatrix);
    glDrawElements(GL_LINES, _gridInfo[_GRID_X_AXIS].vertexCount, GL_UNSIGNED_INT, 0);
    
    /* y-axis */
    glBindVertexArray(_gridInfo[_GRID_Y_AXIS].vaoID);
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, glm::max(camera.getPos().y, _far), 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, _far, 1.0f));
    _gridShader.setMat4("modelMatrix", modelMatrix);
    glDrawElements(GL_LINES, _gridInfo[_GRID_Y_AXIS].vertexCount, GL_UNSIGNED_INT, 0);
    
    /* z-axis */
    glBindVertexArray(_gridInfo[_GRID_Z_AXIS].vaoID);
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 1.0f, glm::max(camera.getPos().z, _far)));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 1.0f, _far));
    _gridShader.setMat4("modelMatrix", modelMatrix);
    glDrawElements(GL_LINES, _gridInfo[_GRID_Z_AXIS].vertexCount, GL_UNSIGNED_INT, 0);
    
    /* Squares */
    glBindVertexArray(_gridInfo[_GRID_SQUARE].vaoID);
    modelMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(round(camera.getPos().x) - _far, 0.0f, round(camera.getPos().z) - _far));
    _gridShader.setMat4("modelMatrix", modelMatrix);
    glDrawElements(GL_LINES, _gridInfo[_GRID_SQUARE].vertexCount, GL_UNSIGNED_INT, 0);
}
