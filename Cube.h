#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <stdint.h>

typedef struct {
    float x, y, z;           // Position
    float r, g, b;           // Color
    float sx, sy, sz;        // Scale
    float rotX, rotY, rotZ;  // Rotation in radians
} CubeObject;

void CUBE_setPosition(CubeObject *cube, int x, int y, int z);
void CUBE_setColor(CubeObject *cube, int r, int g, int b);
void CUBE_draw();
void CUBE_drawArray(CubeObject obj[], int size);
