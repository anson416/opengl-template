#pragma once

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"

class Camera
{
public:
    enum CameraMovement {
        FORWARD,
        BACKWARD,
        LEFTWARD,
        RIGHTWARD,
        UPWARD,
        DOWNWARD,
    };
    
    Camera(GLint scrWidth, GLint scrHeight, glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 front = glm::vec3(0.0f, 0.0f, -1.0f));
    void move(CameraMovement movement, GLfloat deltaTime);
    void view(GLfloat xpos, GLfloat ypos);
    void zoom(GLfloat yoffset);
    glm::mat4 getViewMatrix(void) const;
    glm::vec3 getPos(void) const;
    GLfloat getFOV(void) const;
    void setFirstMouse(GLboolean value);

private:
    GLboolean _firstMouse = GL_TRUE;
    glm::vec3 _cameraPos, _cameraFront, _cameraUp, _cameraRight;
    GLfloat _cameraSpeed;
    GLfloat _yaw, _pitch;
    const GLfloat _TWO_PI = glm::radians(360.0f);
    const GLfloat _MIN_PITCH = glm::radians(-89.0f);
    const GLfloat _MAX_PITCH = glm::radians(89.0f);
    GLfloat _lastX, _lastY;
    GLfloat _sensitivity;
    GLfloat _fov, _minFOV, _maxFOV;
    GLboolean _useCameraSpace;
};
