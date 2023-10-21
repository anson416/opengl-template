#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "camera/camera.h"
#include "grid/grid.h"
#include "shader/shader.h"
#include "misc/misc.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <cmath>

/* ----- Modify macros here ----- */
#define TITLE "OpenGL Template"
#define SCR_WIDTH  800
#define SCR_HEIGHT 600
#define FAR 100
/* ------------------------------ */

#define N_GLFW_KEYS 348

enum Object {
    /* ----- Add objects here ----- */

    /* ---------------------------- */
    OBJECT_COUNT,
};

struct ObjectInfo {
    GLuint vaoID;
    GLsizei vertexCount;
    GLuint textureID;
};
ObjectInfo* objectInfo;

Shader textureShader;
Grid grid;

const GLfloat far = static_cast<GLfloat>(FAR);

GLfloat deltaTime = 0.0f;  /* Time between current frame and last frame */
GLfloat lastFrame = 0.0f;  /* Time of last frame */

GLint scrWidth  = SCR_WIDTH;
GLint scrHeight = SCR_HEIGHT;
GLboolean keys[N_GLFW_KEYS];
GLboolean showGrid = GL_FALSE;

Camera camera(scrWidth, scrHeight, glm::vec3(2.0f), glm::vec3(1.0f, 0.0f, 1.0f));
GLboolean cursorDisabled = GL_TRUE;

/* ----- Function prototypes ----- */
void sendObject(void);
void sendObjectsToOpenGL(void);
void initializeGL(void);
void paintGL(void);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void smoothKeyCallback(void);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
/* ------------------------------- */

int main(int argc, char* argv[])
{
    /* Allocate memory */
    objectInfo = new(std::nothrow) ObjectInfo[OBJECT_COUNT];
    if (!objectInfo) {
        std::cerr << "ERR: Failed to allocate memory" << std::endl;
        return -1;
    }
    
    /* Initialize GLFW */
    if (!glfwInit()) {
        std::cerr << "ERR: Failed to initialize GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    /* Configure GLFW for Mac */
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    
    // glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);  /* Disable resizing */
    glfwWindowHint(GLFW_SAMPLES, 4);  /* Use 4x MSAA */

    /* Create a window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(scrWidth, scrHeight, TITLE, NULL, NULL);
    if (!window) {
        std::cerr << "ERR: Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);  /* Make the window's context current */
    glfwSwapInterval(1);  /* Enable v-sync */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  /* Disable cursor */

    /* Register callback functions */
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorPosCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetScrollCallback(window, scrollCallback);
    
    /* Initialize GLEW */
    if (glewInit() != GLEW_OK) {
        std::cerr << "ERR: Failed to initialize GLEW" << std::endl;
        glfwTerminate();
        return -1;
    }

    showOpenGLInfo();
    initializeGL();

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window)) {
        paintGL();  /* Render here */
        glfwSwapBuffers(window);  /* Swap front and back buffers */
        glfwPollEvents();  /* Poll for and process events */
    }
    
    delete[] objectInfo;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

void sendObject(void)
{

}

void sendObjectsToOpenGL(void)
{

}

void initializeGL(void)
{
    textureShader.setupShader("shaders/texture/texture.vs", "shaders/texture/texture.fs");
    grid.setupGrid("shaders/grid/grid.vs", "shaders/grid/grid.fs", far);
    grid.sendGridsToOpenGL();

    sendObjectsToOpenGL();

    /* Customize 1D and 2D objects */
    glEnable(GL_POINT_SMOOTH);
    glPointSize(10.0f);
    glLineWidth(1.5f);
    
    glEnable(GL_DEPTH_TEST);   /* Realize occlusion */
    glEnable(GL_CULL_FACE);    /* Enable face culling */
    glEnable(GL_MULTISAMPLE);  /* Enable MSAA */
}

void paintGL(void)
{
    GLfloat currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    glClearColor(R(51), G(51), B(51), 1.0f);  /* Specify the background color */
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    smoothKeyCallback();

    glm::mat4 modelMatrix;
    glm::mat4 viewMatrix = camera.getViewMatrix();
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(camera.getFOV()), static_cast<GLfloat>(scrWidth) / static_cast<GLfloat>(scrHeight), 0.01f, static_cast<GLfloat>(FAR));

    if (showGrid)
        grid.drawGrids(viewMatrix, projectionMatrix, camera);

    /* ----- Draw here ----- */

    /* --------------------- */
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    
    /* Ensure window width and height are saved after fullscreen mode is enabled */
    // if (!fullscreenEnabled) {
    //     windowWidth = width;
    //     windowHeight = height;
    // }
}

/* Set the Keyboard callback for the current window */
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    /* Record pressed or released keys for smoothKeyCallback() */
    if (action == GLFW_PRESS || action == GLFW_RELEASE)
        keys[key] = !keys[key];
    
    /* Exit */
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    /* Enable/disable cursor */
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        cursorDisabled = !cursorDisabled;
        camera.setFirstMouse(cursorDisabled ? GL_TRUE : GL_FALSE);
        glfwSetInputMode(window, GLFW_CURSOR, cursorDisabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
    }
    
    /* Enable/disable fullscreen mode */
    /* Reference: https://gamedev.stackexchange.com/questions/58547/how-to-set-to-fullscreen-in-glfw3 */
//    if (key == GLFW_KEY_F && action == GLFW_PRESS) {
//        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//        const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
//        GLFWwindow* currentWindow = glfwGetCurrentContext();
//        
//        firstMouse = true;
//        if (!glfwGetWindowMonitor(currentWindow)) {
//            fullscreenEnabled = true;
//            glfwGetWindowPos(window, &windowXPos, &windowYPos);
//            glfwSetWindowMonitor(currentWindow, monitor, 0, 0, videoMode->width, videoMode->height, videoMode->refreshRate);
//        }
//        else {
//            glfwSetWindowMonitor(currentWindow, NULL, windowXPos, windowYPos, windowWidth, windowHeight, GLFW_DONT_CARE);
//            fullscreenEnabled = false;
//        }
//    }
    
    /* Enable/disable grid mode */
    if (key == GLFW_KEY_G && action == GLFW_PRESS)
        showGrid = !showGrid;
}

void smoothKeyCallback(void)
{
    if (keys[GLFW_KEY_W])
        camera.move(camera.FORWARD, deltaTime);
    if (keys[GLFW_KEY_S])
        camera.move(camera.BACKWARD, deltaTime);
    if (keys[GLFW_KEY_A])
        camera.move(camera.LEFTWARD, deltaTime);
    if (keys[GLFW_KEY_D])
        camera.move(camera.RIGHTWARD, deltaTime);
    if (keys[GLFW_KEY_SPACE])
        camera.move(camera.UPWARD, deltaTime);
    if (keys[GLFW_KEY_LEFT_SHIFT] || keys[GLFW_KEY_RIGHT_SHIFT])
        camera.move(camera.DOWNWARD, deltaTime);
}

/* Set the cursor position callback for the current window */
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    if (cursorDisabled)
        camera.view(static_cast<GLfloat>(xpos), static_cast<GLfloat>(ypos));
}

/* Set the mouse-button callback for the current window */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    
}

/* Set the scoll callback for the current window */
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.zoom(static_cast<GLfloat>(yoffset));
}
