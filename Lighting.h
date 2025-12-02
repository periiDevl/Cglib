#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct
{
    int x, y, z, r, g, b;
} LightSource;

void LIGHT_enable();
void LIGHT_apply(LightSource lights[], int size);
void LIGHT_ambient(float r, float g, float b);