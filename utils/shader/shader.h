#pragma once

#include "GL/glew.h"
#include "glm/glm.hpp"

#include <string>
#include <iostream>

class Shader {
public:
	void setupShader(const char* vertexPath, const char* fragmentPath);
    void use() const;
    void setBool(const std::string& name, GLboolean value) const;
    void setInt(const std::string& name, GLint value) const;
    void setFloat(const std::string& name, GLfloat value) const;
    void setVec2(const std::string& name, glm::vec2 value) const;
    void setVec2(const std::string& name, GLfloat x, GLfloat y) const;
    void setVec3(const std::string& name, glm::vec3 value) const;
    void setVec3(const std::string& name, GLfloat x, GLfloat y, GLfloat z) const;
    void setVec4(const std::string& name, glm::vec4 value) const;
    void setVec4(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const;
    void setMat2(const std::string& name, glm::mat2 value) const;
    void setMat3(const std::string& name, glm::mat3 value) const;
    void setMat4(const std::string& name, glm::mat4 value) const;

private:
	unsigned int _ID;

	std::string _readShaderCode(const char* fileName) const;
	bool _checkShaderStatus(GLuint shaderID) const;
	bool _checkProgramStatus(GLuint programID) const;
	bool _checkStatus(
		GLuint objectID,
		PFNGLGETSHADERIVPROC objectPropertyGetterFunc,
		PFNGLGETSHADERINFOLOGPROC getInfoLogFunc,
		GLenum statusType) const;
    GLint _getUniformLocation(const std::string& name) const;
};
