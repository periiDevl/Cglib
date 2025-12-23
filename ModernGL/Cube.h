#pragma once
#include "cglm/cglm.h"
typedef struct {
    vec3 position;
    vec3 scale;
    vec3 rotation;
    vec4 color;

    float ambientStrength;
    float diffuseStrength;
    float specularStrength;
    float shininess;
} Cube;
