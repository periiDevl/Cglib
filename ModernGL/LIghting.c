#include"Lighting.h"
unsigned int shadowShaderProgram; 
const unsigned int SHADOW_WIDTH = 1024;
const unsigned int SHADOW_HEIGHT = 1024;
unsigned int depthMapFBO[3];
unsigned int depthCubemaps[3];
    
void SHADOWS_compileShaderForShadows()
{
    shadowShaderProgram=compileShaderProgramWithGeometry(
        shadowDepthVertexShaderSource, 
        shadowDepthFragmentShaderSource,
        shadowDepthGeometryShaderSource
    );
}
void SHADOWS_createShadowMapAndCubemaps()
{

    for(int i = 0; i < 3; i++) {
        glGenFramebuffers(1, &depthMapFBO[i]);
        glGenTextures(1, &depthCubemaps[i]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemaps[i]);
        
        for(unsigned int j = 0; j < 6; ++j) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT,
                        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }
        
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemaps[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Shadow FBO %d not complete!\n", i);
            
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}
void SHADOWS_shadowPassSetup(Light* lights, int lightIdx, float farPlane)
{
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[lightIdx]);
    glClear(GL_DEPTH_BUFFER_BIT);
    
    mat4 shadowProj;
    glm_perspective(glm_rad(90.0f), 1.0f, 0.1f, farPlane, shadowProj);
    
    vec3 targets[6] = {
        {1.0f, 0.0f, 0.0f}, {-1.0f, 0.0f, 0.0f},
        {0.0f, 1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f}
    };
    vec3 ups[6] = {
        {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f},
        {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f, -1.0f},
        {0.0f, -1.0f, 0.0f}, {0.0f, -1.0f, 0.0f}
    };
    
    mat4 shadowTransforms[6];
    for(int i = 0; i < 6; i++) {
        mat4 shadowView;
        vec3 target;
        glm_vec3_add(lights[lightIdx].position, targets[i], target);
        glm_lookat(lights[lightIdx].position, target, ups[i], shadowView);
        glm_mat4_mul(shadowProj, shadowView, shadowTransforms[i]);
    }
    
    glUseProgram(shadowShaderProgram);
    for(int i = 0; i < 6; i++) {
        char name[32];
        sprintf(name, "shadowMatrices[%d]", i);
        glUniformMatrix4fv(glGetUniformLocation(shadowShaderProgram, name), 1, GL_FALSE, (float*)shadowTransforms[i]);
    }
    glUniform3fv(glGetUniformLocation(shadowShaderProgram, "lightPos"), 1, lights[lightIdx].position);
    glUniform1f(glGetUniformLocation(shadowShaderProgram, "farPlane"), farPlane);
    
}
void LIGHTING_sendLightsToShader(unsigned int* shaderProgram, Light* lights, int numLights) {
    for(int i = 0; i < numLights; i++) {
        char uniformName[64];
        sprintf(uniformName, "lights[%d].position", i);
        glUniform3fv(glGetUniformLocation(*shaderProgram, uniformName), 1, lights[i].position);
        sprintf(uniformName, "lights[%d].color", i);
        glUniform3fv(glGetUniformLocation(*shaderProgram, uniformName), 1, lights[i].color);
        sprintf(uniformName, "lights[%d].ambient", i);
        glUniform1f(glGetUniformLocation(*shaderProgram, uniformName), lights[i].ambient);
        sprintf(uniformName, "lights[%d].diffuse", i);
        glUniform1f(glGetUniformLocation(*shaderProgram, uniformName), lights[i].diffuse);
        sprintf(uniformName, "lights[%d].specular", i);
        glUniform1f(glGetUniformLocation(*shaderProgram, uniformName), lights[i].specular);
        sprintf(uniformName, "lights[%d].constant", i);
        glUniform1f(glGetUniformLocation(*shaderProgram, uniformName), lights[i].constant);
        sprintf(uniformName, "lights[%d].linear", i);
        glUniform1f(glGetUniformLocation(*shaderProgram, uniformName), lights[i].linear);
        sprintf(uniformName, "lights[%d].quadratic", i);
        glUniform1f(glGetUniformLocation(*shaderProgram, uniformName), lights[i].quadratic);
    }
}
void SHADOWS_dynamicShadowMaps(Light* lights, int lightSize, unsigned int* shader)
{
    for(int i = 0; i < lightSize; i++) {
        char name[32];
        sprintf(name, "shadowMaps[%d]", i);
        glUniform1i(glGetUniformLocation(*shader, name), 3 + i);
        glActiveTexture(GL_TEXTURE3 + i);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemaps[i]);
    }
}