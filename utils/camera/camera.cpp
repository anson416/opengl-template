#include "camera.h"

#include "glm/gtc/matrix_transform.hpp"

#include "misc/misc.h"

#include <iostream>

Camera::Camera(GLint scrWidth, GLint scrHeight, glm::vec3 pos, glm::vec3 front)
{
    _firstMouse = GL_TRUE;
    _cameraPos = pos;
    _cameraFront = glm::normalize(front);
    _cameraRight = glm::normalize(glm::cross(_cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    _cameraUp = glm::cross(_cameraRight, _cameraFront);
    _cameraSpeed = 5.0f;
    _yaw = glm::sign(_cameraFront.z) * glm::acos(glm::normalize(glm::vec3(_cameraFront.x, 0.0f, _cameraFront.z)).x);
    _pitch = glm::sign(_cameraFront.y) * glm::acos(_cameraUp.y);
    _lastX = static_cast<GLfloat>(scrWidth) / 2.0f;
    _lastY = static_cast<GLfloat>(scrHeight) / 2.0f;
    _sensitivity = 0.003f;
    _fov = 45.0f;
    _minFOV = 1.0f;
    _maxFOV = 45.0f;
    _useCameraSpace = GL_FALSE;
}

void Camera::move(Camera::CameraMovement movement, GLfloat deltaTime)
{
    GLfloat movementSpeed = _cameraSpeed * deltaTime;
    if (movement == FORWARD)
        _cameraPos += glm::normalize(glm::vec3(_cameraFront.x, 0.0f, _cameraFront.z)) * movementSpeed;
    if (movement == BACKWARD)
        _cameraPos -= glm::normalize(glm::vec3(_cameraFront.x, 0.0f, _cameraFront.z)) * movementSpeed;
    if (movement == LEFTWARD)
        _cameraPos -= _cameraRight * movementSpeed;
    if (movement == RIGHTWARD)
        _cameraPos += _cameraRight * movementSpeed;
    if (movement == UPWARD) {
        if (_useCameraSpace)
            _cameraPos += _cameraUp * movementSpeed;
        else
            _cameraPos += glm::vec3(0.0f, movementSpeed, 0.0f);
    }
    if (movement == DOWNWARD) {
        if (_useCameraSpace)
            _cameraPos -= _cameraUp * movementSpeed;
        else
            _cameraPos -= glm::vec3(0.0f, movementSpeed, 0.0f);
    }
}

void Camera::view(GLfloat xpos, GLfloat ypos)
{
    if (_firstMouse) {
        _lastX = xpos;
        _lastY = ypos;
        _firstMouse = GL_FALSE;
    }

    GLfloat xoffset = (xpos - _lastX) * _sensitivity;
    GLfloat yoffset = (_lastY - ypos) * _sensitivity;

    _lastX = xpos;
    _lastY = ypos;
    
    _yaw   = glm::mod(_yaw + xoffset, _TWO_PI);
    _pitch = CLAMP(_pitch + yoffset, _MIN_PITCH, _MAX_PITCH);  /* Restrict pitch */
    
    /* Compute new camera directions */
    _cameraFront.x = cos(_yaw) * cos(_pitch);
    _cameraFront.y = sin(_pitch);
    _cameraFront.z = sin(_yaw) * cos(_pitch);
    _cameraFront = glm::normalize(_cameraFront);
    _cameraRight = glm::normalize(glm::cross(_cameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));
    _cameraUp = glm::cross(_cameraRight, _cameraFront);
}

void Camera::zoom(GLfloat yoffset)
{
    _fov = CLAMP(_fov - yoffset, _minFOV, _maxFOV);  /* Change FOV on mouse scroll */
}

glm::mat4 Camera::getViewMatrix(void) const
{
    return glm::lookAt(_cameraPos, _cameraPos + _cameraFront, _cameraUp);
}

glm::vec3 Camera::getPos(void) const
{
    return _cameraPos;
}

GLfloat Camera::getFOV(void) const
{
    return _fov;
}

void Camera::setFirstMouse(GLboolean value)
{
    _firstMouse = value;
}
