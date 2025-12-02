#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdio.h>
#include"Cube.h"
#include <stdlib.h>
#include"Lighting.h"
#include"Camera.h"

CubeObject objects[] = {
    {0,0,0,1,1,1},
    {1,1,1,1,0,0},
};
LightSource lights[] =
{
    {6,6,0,1,1,1},
    {0,0,0,0.6f,0.6f,0.6f},
};
Camera* camera;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    CAMERA_mouse(camera, xpos, ypos);
}

int main() {
    camera = malloc(sizeof(Camera));

    CAMERA_init(camera);
    if (!glfwInit()) return -1;
    GLFWwindow* window = glfwCreateWindow(800, 600, "cglib window", NULL, NULL);
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
        lights[1].x = camera->camX;
        lights[1].y = camera->camY;
        lights[1].z = camera->camZ;

        glClearColor(0.1,0.1,0.2,1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        CAMERA_perspective(camera, 800, 600);
        LIGHT_apply(lights,2);
        CUBE_drawArray(objects, 2);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
