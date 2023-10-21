#include "shader.h"

#include "glm/gtc/type_ptr.hpp"

#include <fstream>

void Shader::setupShader(const char* vertexPath, const char* fragmentPath)
{
    unsigned int vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    const GLchar* vCode;
    std::cout << "INF: Loading vertex shader " << vertexPath << "..." << std::endl;
    std::string temp = _readShaderCode(vertexPath);
    vCode = temp.c_str();
    glShaderSource(vertexShaderID, 1, &vCode, NULL);
    glCompileShader(vertexShaderID);
    if (!_checkShaderStatus(vertexShaderID)) {
        std::cerr << "ERR: Failed to compile vertex shader " << vertexPath << std::endl;
        exit(1);
    }

    const GLchar* fCode;
    std::cout << "INF: Loading fragment shader " << fragmentPath << "..." << std::endl;
    temp = _readShaderCode(fragmentPath);
    fCode = temp.c_str();
    glShaderSource(fragmentShaderID, 1, &fCode, NULL);
    glCompileShader(fragmentShaderID);
    if (!_checkShaderStatus(fragmentShaderID)) {
        std::cerr << "ERR: Failed to compile fragment shader " << fragmentPath << std::endl;
        exit(1);
    }

    _ID = glCreateProgram();

    glAttachShader(_ID, vertexShaderID);
    glAttachShader(_ID, fragmentShaderID);
    glLinkProgram(_ID);

    if (!_checkProgramStatus(_ID))
        return;

    glDeleteShader(vertexShaderID);
    glDeleteShader(fragmentShaderID);

    glUseProgram(0);
}

void Shader::use() const
{
	glUseProgram(_ID);
}

void Shader::setBool(const std::string& name, GLboolean value) const
{
    glUniform1i(_getUniformLocation(name), (GLint)value);
}

void Shader::setInt(const std::string& name, GLint value) const
{
    glUniform1i(_getUniformLocation(name), value);
}

void Shader::setFloat(const std::string& name, GLfloat value) const
{
    glUniform1f(_getUniformLocation(name), value);
}

void Shader::setVec2(const std::string& name, glm::vec2 value) const
{
    glUniform2fv(_getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec2(const std::string& name, GLfloat x, GLfloat y) const
{
    glUniform2f(_getUniformLocation(name), x, y);
}

void Shader::setVec3(const std::string& name, glm::vec3 value) const
{
    glUniform3fv(_getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec3(const std::string& name, GLfloat x, GLfloat y, GLfloat z) const
{
    glUniform3f(_getUniformLocation(name), x, y, z);
}

void Shader::setVec4(const std::string& name, glm::vec4 value) const
{
    glUniform4fv(_getUniformLocation(name), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string& name, GLfloat x, GLfloat y, GLfloat z, GLfloat w) const
{
    glUniform4f(_getUniformLocation(name), x, y, z, w);
}

void Shader::setMat2(const std::string& name, glm::mat2 value) const
{
    glUniformMatrix2fv(_getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat3(const std::string& name, glm::mat3 value) const
{
    glUniformMatrix3fv(_getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string& name, glm::mat4 value) const
{
	glUniformMatrix4fv(_getUniformLocation(name), 1, GL_FALSE, glm::value_ptr(value));
}

std::string Shader::_readShaderCode(const char* fileName) const
{
	std::ifstream myInput(fileName);
	if (!myInput.good())
	{
		std::cout << "ERR: Failed to load " << fileName << std::endl;
		exit(1);
	}
	return std::string(
		std::istreambuf_iterator<char>(myInput),
		std::istreambuf_iterator<char>()
	);
}

bool Shader::_checkShaderStatus(GLuint shaderID) const
{
	return _checkStatus(shaderID, glGetShaderiv, glGetShaderInfoLog, GL_COMPILE_STATUS);
}

bool Shader::_checkProgramStatus(GLuint programID) const
{
	return _checkStatus(programID, glGetProgramiv, glGetProgramInfoLog, GL_LINK_STATUS);
}

bool Shader::_checkStatus(GLuint objectID, PFNGLGETSHADERIVPROC objectPropertyGetterFunc, PFNGLGETSHADERINFOLOGPROC getInfoLogFunc, GLenum statusType) const
{
	GLint status;
	objectPropertyGetterFunc(objectID, statusType, &status);
	if (status != GL_TRUE)
	{
		GLint infoLogLength;
		objectPropertyGetterFunc(objectID, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* buffer = new GLchar[infoLogLength];

		GLsizei bufferSize;
		getInfoLogFunc(objectID, infoLogLength, &bufferSize, buffer);
		std::cout << buffer << std::endl;

		delete[] buffer;
		return false;
	}
	return true;
}

GLint Shader::_getUniformLocation(const std::string& name) const
{
    return glGetUniformLocation(_ID, name.c_str());
}
