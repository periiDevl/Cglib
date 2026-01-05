#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "cglm/cglm.h"
#include"ShaderScripts.h"
#include"ShaderHelper.h"
typedef struct {
    vec3 position;
    vec3 color;
    float ambient;
    float diffuse;
    float specular;
    float constant; 
    float linear;   
    float quadratic;
} Light;
//LIGHTS:
void LIGHTING_sendLightsToShader(unsigned int* shader,Light* lights, int numLights);
//SHADOWS:
extern unsigned int shadowShaderProgram;
extern const unsigned int SHADOW_WIDTH;
extern const unsigned int SHADOW_HEIGHT;
//I need to change this from 3 to number of light.
extern unsigned int depthMapFBO[3];
extern unsigned int depthCubemaps[3];
    
void SHADOWS_compileShaderForShadows();
void SHADOWS_createShadowMapAndCubemaps();
void SHADOWS_shadowPassSetup(Light* lights, int lightIdx, float farPlane);
void SHADOWS_dynamicShadowMaps(Light* lights, int lightSize, unsigned int* shader);