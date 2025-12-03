#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include"Cube.h"
#include <stdlib.h>
#include"Lighting.h"
#include"Camera.h"

CubeObject objects[] = {
    {0,0,0,0.7,0.7,0.7,3,3,3},
    {2,3,4,0.15,0.5,0.2, 2, 2, 4},
    {-3,-3,-3,0.15,0.5,0.2,3,1,2},
};
LightSource lights[] =
{
    {6,6,6,4,4,4,0,0},
    {0,0,0,5.0f,5.0f,5.0f, 0.4f, 0.1f},
};
Camera* camera;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    CAMERA_mouse(camera, xpos, ypos);
}

int main() {
    camera = malloc(sizeof(Camera));

    CAMERA_init(camera);
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "cglib window", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }

    glfwMakeContextCurrent(window);
    glewInit();

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);

    LIGHT_enable();

    LIGHT_ambient(0.1f,0.1f,0.1f);
    while (!glfwWindowShouldClose(window)) {
        CAMERA_movement(camera, window);
        
        glClearColor(0.2,0.2,0.2,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        CAMERA_perspective(camera, 1280, 720);
        LIGHT_apply(lights,sizeof(lights)/sizeof(LightSource));
        CUBE_drawArray(objects, sizeof(objects)/sizeof(CubeObject));
        lights[1].x = camera->camX;
        lights[1].y = camera->camY;
        lights[1].z = camera->camZ;

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
