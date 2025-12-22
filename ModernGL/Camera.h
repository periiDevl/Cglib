#pragma once
#include "cglm/cglm.h"
#include "GLFW/glfw3.h"

typedef struct {
    vec3 cameraPos;
    vec3 cameraFront;
    vec3 cameraUp;

    float lastX;
    float lastY;
    float yaw;
    float pitch;
    int firstMouse;
    double speed;
} Camera;
void CAMERA_init(Camera* camera);
void CAMERA_mouseRotation(Camera* camera,GLFWwindow* window, double xpos, double ypos);
void CAMERA_wasd(Camera* camera, GLFWwindow* window, double deltaTime);

