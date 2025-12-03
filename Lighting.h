#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

typedef struct
{
    float x, y, z, r, g, b;
    float linearFallOff, QuadraticFalloff;
} LightSource;

void LIGHT_enable();
void LIGHT_apply(LightSource lights[], int size);
void LIGHT_ambient(float r, float g, float b);