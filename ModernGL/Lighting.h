#pragma once
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
typedef struct {
    vec3 position;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
} Light;