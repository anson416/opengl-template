#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "camera/camera.h"
#include "grid/grid.h"
#include "misc/misc.h"
#include "shader/shader.h"
#include "skybox/skybox.h"
#include "texture/texture.h"

#include <iostream>
#include <vector>

/* ----- Modify macros ----- */
#define TITLE "OpenGL Template"
#define SCR_WIDTH  800
#define SCR_HEIGHT 600
#define FAR 100
/* ------------------------- */

#define N_GLFW_KEYS 348

enum Object {
    /* ----- Add objects ----- */
    SPHERE,
    IRON_MAN,
    /* ----------------------- */
    OBJECT_COUNT,
};

struct ObjectInfo {
    GLuint vaoID;
    GLsizei vertexCount;
    Texture tex_diffuse, tex_specular;
};
ObjectInfo* objectInfo;

Shader textureShader;
Grid grid;
Skybox skybox;

const GLfloat far = static_cast<GLfloat>(FAR);

GLfloat deltaTime = 0.0f;  /* Time between current frame and last frame */
GLfloat lastFrame = 0.0f;  /* Time of last frame */

GLint scrWidth  = SCR_WIDTH;
GLint scrHeight = SCR_HEIGHT;
GLboolean keys[N_GLFW_KEYS];
GLboolean showGrid = GL_FALSE;

Camera camera(scrWidth, scrHeight, glm::vec3(0.0f, 3.0f, 8.0f), glm::vec3(0.0f, -0.3f, -1.0f));
GLboolean cursorDisabled = GL_TRUE;

/* ----- Define function prototypes ----- */
void paintGL(void);
void sendObjectsToOpenGL(void);
void initializeGL(void);
void sendObject(GLuint objectID, const char* objPath, GLuint* vboID, GLuint* eboID);
void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void smoothKeyCallback(void);
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
/* -------------------------------------- */

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
        paintGL();  /* Render */
        glfwSwapBuffers(window);  /* Swap front and back buffers */
        glfwPollEvents();  /* Poll for and process events */
    }
    
    delete[] objectInfo;

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
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
        grid.draw(viewMatrix, projectionMatrix, camera);

    textureShader.use();
    textureShader.setMat4("viewMatrix", viewMatrix);
    textureShader.setMat4("projectionMatrix", projectionMatrix);
    textureShader.setVec3("eyePosWorld", camera.getPos());

    /* ----- Modify texture shader ----- */
    glm::vec3 pointLightPos = glm::vec3(0.0f, 8.0f, 10.0f);

    textureShader.setBool("useBlinn", GL_TRUE);
    textureShader.setVec3("emissionK", glm::vec3(0.0f));
    textureShader.setVec3("ambientK", glm::vec3(0.1f));
    textureShader.setVec3("pointLights[0].light.diffuseK", glm::vec3(0.4f));
    textureShader.setVec3("pointLights[0].light.specularK", glm::vec3(0.5f));
    textureShader.setVec3("pointLights[0].light.intensity", glm::vec3(5.0f));
    textureShader.setVec3("pointLights[0].pos", pointLightPos);
    textureShader.setFloat("pointLights[0].attenuation.a", 1.0f);
    textureShader.setFloat("pointLights[0].attenuation.b", 0.01f);
    textureShader.setFloat("pointLights[0].attenuation.c", 0.001f);
    textureShader.setVec3("dirLights[0].light.diffuseK", glm::vec3(0.4f));
    textureShader.setVec3("dirLights[0].light.specularK", glm::vec3(0.5f));
    textureShader.setVec3("dirLights[0].light.intensity", glm::vec3(2.0f));
    textureShader.setVec3("dirLights[0].dir", glm::vec3(0.0f, 0.0f, 1.0f));
    textureShader.setVec3("dirLights[1].light.diffuseK", glm::vec3(0.4f));
    textureShader.setVec3("dirLights[1].light.specularK", glm::vec3(0.5f));
    textureShader.setVec3("dirLights[1].light.intensity", glm::vec3(2.0f));
    textureShader.setVec3("dirLights[1].dir", glm::vec3(-1.0f, 0.0f, 0.0f));
    textureShader.setVec3("dirLights[2].light.diffuseK", glm::vec3(0.4f));
    textureShader.setVec3("dirLights[2].light.specularK", glm::vec3(0.5f));
    textureShader.setVec3("dirLights[2].light.intensity", glm::vec3(2.0f));
    textureShader.setVec3("dirLights[2].dir", glm::vec3(1.0f, 0.0f, 0.0f));
    /* --------------------------------- */

    /* ----- Draw non-luminous objects ----- */
    glBindVertexArray(objectInfo[IRON_MAN].vaoID);
    objectInfo[IRON_MAN].tex_diffuse.bind(0);
    textureShader.setInt("material.diffuse", 0);
    objectInfo[IRON_MAN].tex_specular.bind(1);
    textureShader.setInt("material.specular", 1);
    textureShader.setFloat("material.shininess", 64);
    modelMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));
    textureShader.setMat4("modelMatrix", modelMatrix);
    glDrawElements(GL_TRIANGLES, objectInfo[IRON_MAN].vertexCount, GL_UNSIGNED_INT, 0);
    /* ------------------------------------- */

    textureShader.setVec3("emissionK", glm::vec3(1.0f));
    
    /* ----- Draw luminous objects ----- */
    glBindVertexArray(objectInfo[SPHERE].vaoID);
    objectInfo[SPHERE].tex_diffuse.bind(0);
    textureShader.setInt("material.diffuse", 0);
    objectInfo[SPHERE].tex_specular.bind(1);
    textureShader.setInt("material.specular", 1);
    textureShader.setFloat("material.shininess", 32);
    modelMatrix = glm::translate(glm::mat4(1.0f), pointLightPos);
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8f, 0.8f, 0.8f));
    textureShader.setMat4("modelMatrix", modelMatrix);
    glDrawElements(GL_TRIANGLES, objectInfo[SPHERE].vertexCount, GL_UNSIGNED_INT, 0);
    /* --------------------------------- */

    skybox.draw(viewMatrix, projectionMatrix);
}

void sendObjectsToOpenGL(void)
{
    GLuint vboID, eboID;

    /* ----- Load objects and textures ----- */
    /* Credit: https://github.com/melfm/openGL-shading-texture/tree/master */
    sendObject(SPHERE, "resources/sphere/sphere.obj", &vboID, &eboID);
    objectInfo[SPHERE].tex_diffuse.setupTexture("resources/sphere/sphere_diffuse.jpg");
    objectInfo[SPHERE].tex_specular.setupTexture("resources/sphere/sphere_specular.jpg");

    /* Credit: https://sketchfab.com/3d-models/iron-man-rig-a921a8cac309424e939aee1d31fa28c0 */
    sendObject(IRON_MAN, "resources/iron-man/iron-man.obj", &vboID, &eboID);
    objectInfo[IRON_MAN].tex_diffuse.setupTexture("resources/iron-man/iron-man_diffuse.png");
    objectInfo[IRON_MAN].tex_specular.setupTexture("resources/iron-man/iron-man_specular.png");
    /* ------------------------------------- */
}

void initializeGL(void)
{
    textureShader.setupShader("shaders/texture/texture.vs", "shaders/texture/texture.fs");

    grid.setupGrid("shaders/grid/grid.vs", "shaders/grid/grid.fs", far);
    grid.sendGridsToOpenGL();

    /* Credit: https://learnopengl.com/Advanced-OpenGL/Cubemaps */
    const std::vector<std::string> skyboxTexPaths = {
        "resources/skybox/right.jpg",
        "resources/skybox/left.jpg",
        "resources/skybox/top.jpg",
        "resources/skybox/bottom.jpg",
        "resources/skybox/front.jpg",
        "resources/skybox/back.jpg",
    };
    skybox.setupSkybox("shaders/skybox/skybox.vs", "shaders/skybox/skybox.fs", skyboxTexPaths);

    sendObjectsToOpenGL();

    /* Customize 1D and 2D objects */
    glEnable(GL_POINT_SMOOTH);
    glPointSize(10.0f);
    glLineWidth(1.5f);
    
    glEnable(GL_DEPTH_TEST);   /* Realize occlusion */
    glEnable(GL_CULL_FACE);    /* Enable face culling */
    glEnable(GL_MULTISAMPLE);  /* Enable MSAA */
}

void sendObject(GLuint objectID, const char* objPath, GLuint* vboID, GLuint* eboID)
{
    Model obj = loadOBJ(objPath);
    
    glGenVertexArrays(1, &objectInfo[objectID].vaoID);
    glBindVertexArray(objectInfo[objectID].vaoID);
    
    glGenBuffers(1, vboID);
    glBindBuffer(GL_ARRAY_BUFFER, *vboID);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(Vertex), &obj.vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, eboID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), &obj.indices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, pos)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, uv)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, normal)));
    
    objectInfo[objectID].vertexCount = (GLsizei)obj.indices.size();
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
