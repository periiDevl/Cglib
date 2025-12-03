#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
typedef struct
{
    float camX, camY, camZ;
    float yaw, pitch;
    float camForwardX, camForwardY, camForwardZ;
    float fov;
    float speed;
} Camera;

void CAMERA_rotation(Camera* camera);
void CAMERA_init(Camera* camera);
void CAMERA_perspective(Camera* camera, int windowX, int windowY);
void CAMERA_mouse(Camera* camera, double xMouse, double yMouse);
void CAMERA_movement(Camera* camera, GLFWwindow* window);