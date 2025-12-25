#pragma once
#include "glad/glad.h"
#include <stdio.h>

// Compile a standard vertex + fragment shader program
unsigned int compileShaderProgram(const char* vertexSource, const char* fragmentSource);

// Compile a shader program with geometry shader
unsigned int compileShaderProgramWithGeometry(const char* vertexSource, const char* fragmentSource, const char* geometrySource);