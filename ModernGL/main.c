#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "ShaderScripts.h"
#include "HardCodedModels.h"
#include "ShaderHelper.h"
#include "Camera.h"
#include"Cube.h"
#include"Lighting.h"
const int screenX = 1280;
const int screenY = 720;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

float deltaTime = 0.0f;
float lastFrame = 0.0f;
Camera* camera;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    //CAMERA_mouseRotation(camera, window, xpos, ypos);
}

void processInput(GLFWwindow* window) {
    //CAMERA_wasd(camera, window, deltaTime);
}

float lerp(float a, float b, float f) {
    return a + f * (b - a);
}

void CAMERA_loadFromFile(Camera* camera, const char* filename) {
    if (!camera) return;
    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }
    fscanf(file, "%f %f %f", &camera->cameraPos[0], &camera->cameraPos[1], &camera->cameraPos[2]);
    fscanf(file, "%f %f %f", &camera->cameraFront[0], &camera->cameraFront[1], &camera->cameraFront[2]);
    fscanf(file, "%f %f", &camera->yaw, &camera->pitch);
    fscanf(file, "%lf", &camera->speed);
    fclose(file);
}

void renderScene(unsigned int shaderProgram, unsigned int cubeVAO, Cube* cubes, int numCubes) {
    glBindVertexArray(cubeVAO);
    for(int i = 0; i < numCubes; i++) {
        mat4 model;
        glm_mat4_identity(model);
        glm_translate(model, cubes[i].position);
        glm_rotate_y(model, cubes[i].rotation[1], model);
        glm_scale(model, cubes[i].scale);
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

int main() {
    camera = malloc(sizeof(Camera));
    CAMERA_init(camera);
    srand(time(NULL));
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    GLFWwindow* window = glfwCreateWindow(screenX, screenY, "Cglib viewport", NULL, NULL);
    if(!window) { 
        printf("Failed to create window\n"); 
        glfwTerminate(); 
        return -1; 
    }
    glfwMakeContextCurrent(window);
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) { 
        printf("Failed GLAD\n"); 
        return -1; 
    }
    glEnable(GL_MULTISAMPLE);
    glViewport(0, 0, screenX, screenY);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glEnable(GL_DEPTH_TEST);
    printf("OpenGL: %s\n", glGetString(GL_VERSION));
    printf("GLSL: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    // Compile shaders
    unsigned int forwardShaderProgram = compileShaderProgram(forwardVertexShaderSource, forwardFragmentShaderSource);
    unsigned int unlitShaderProgram = compileShaderProgram(unlitVertexShaderSource, unlitFragmentShaderSource);
    unsigned int depthShaderProgram = compileShaderProgram(depthVertexShaderSource, depthFragmentShaderSource);
    unsigned int ssaoShaderProgram = compileShaderProgram(ssaoVertexShaderSource, ssaoFragmentShaderSource);
    unsigned int blurShaderProgram = compileShaderProgram(ssaoVertexShaderSource, blurFragmentShaderSource);
    // Post-processing shaders
    unsigned int brightnessShaderProgram = compileShaderProgram(brightnessVertexShaderSource, brightnessFragmentShaderSource);
    unsigned int gaussianBlurShaderProgram = compileShaderProgram(brightnessVertexShaderSource, gaussianBlurFragmentShaderSource);
    unsigned int postProcShaderProgram = compileShaderProgram(brightnessVertexShaderSource, postProcFragmentShaderSource);
    
    SHADOWS_compileShaderForShadows();
    SHADOWS_createShadowMapAndCubemaps();

    // ============ POST-PROCESSING FRAMEBUFFER SETUP ============
    
    // Main HDR rendering target (MSAA)
    unsigned int hdrMsaaFBO;
    glGenFramebuffers(1, &hdrMsaaFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, hdrMsaaFBO);

    unsigned int hdrMsaaColorBuffer;
    glGenTextures(1, &hdrMsaaColorBuffer);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, hdrMsaaColorBuffer);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, screenX, screenY, GL_TRUE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, hdrMsaaColorBuffer, 0);

    unsigned int hdrMsaaRboDepth;
    glGenRenderbuffers(1, &hdrMsaaRboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, hdrMsaaRboDepth);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH_COMPONENT, screenX, screenY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, hdrMsaaRboDepth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("HDR MSAA FBO not complete!\n");

    // Post-processing FBO (resolved, non-MSAA HDR buffer)
    unsigned int postProcFBO;
    glGenFramebuffers(1, &postProcFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postProcFBO);

    unsigned int postProcColorBuffer;
    glGenTextures(1, &postProcColorBuffer);
    glBindTexture(GL_TEXTURE_2D, postProcColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenX, screenY, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postProcColorBuffer, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("Post-processing FBO not complete!\n");

    // Bloom extraction and blur FBOs (part of post-processing pipeline)
    unsigned int bloomFBO[2];
    unsigned int bloomColorBuffers[2];
    glGenFramebuffers(2, bloomFBO);
    glGenTextures(2, bloomColorBuffers);

    for(int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[i]);
        glBindTexture(GL_TEXTURE_2D, bloomColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenX, screenY, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, bloomColorBuffers[i], 0);
        
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            printf("Bloom FBO %d not complete!\n", i);
    }

    // SSAO geometry buffer (for depth/normals)
    unsigned int ssaoDepthFBO;
    glGenFramebuffers(1, &ssaoDepthFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoDepthFBO);

    unsigned int gPosition, gNormal;
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenX, screenY, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, screenX, screenY, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, screenX, screenY);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("SSAO Depth FBO not complete!\n");

    // SSAO computation and blur FBOs
    unsigned int ssaoFBO, ssaoBlurFBO;
    glGenFramebuffers(1, &ssaoFBO);
    glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    unsigned int ssaoColorBuffer;
    glGenTextures(1, &ssaoColorBuffer);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenX, screenY, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
    unsigned int ssaoColorBufferBlur;
    glGenTextures(1, &ssaoColorBufferBlur);
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screenX, screenY, 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Generate SSAO kernel
    vec3 ssaoKernel[64];
    for(int i = 0; i < 64; ++i) {
        vec3 sample = {
            (float)rand() / RAND_MAX * 2.0f - 1.0f,
            (float)rand() / RAND_MAX * 2.0f - 1.0f,
            (float)rand() / RAND_MAX
        };
        glm_vec3_normalize(sample);
        float scale = (float)i / 64.0f;
        scale = lerp(0.1f, 1.0f, scale * scale);
        glm_vec3_scale(sample, scale, sample);
        glm_vec3_copy(sample, ssaoKernel[i]);
    }

    // Generate noise texture
    vec3 ssaoNoise[16];
    for(int i = 0; i < 16; i++) {
        ssaoNoise[i][0] = (float)rand() / RAND_MAX * 2.0f - 1.0f;
        ssaoNoise[i][1] = (float)rand() / RAND_MAX * 2.0f - 1.0f;
        ssaoNoise[i][2] = 0.0f;
    }

    unsigned int noiseTexture;
    glGenTextures(1, &noiseTexture);
    glBindTexture(GL_TEXTURE_2D, noiseTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Setup VAOs
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindVertexArray(cubeVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4*sizeof(float), (void*)(2*sizeof(float)));
    glEnableVertexAttribArray(1);

    // Create scene with full material properties
    Cube cubes[10];
    Cube lightCubes[3];

    lightCubes[0] = (Cube){ 
        .position = {0.0f, -1.5f, 0.0f}, 
        .scale = {2.0f, 2.0f, 2.0f}, 
        .rotation = {0,0,0}, 
        .color = {1.5f, 1.5f, 1.5f, 1.0f},
        .shininess = 50.0f,
        .diffuseStrength = 1.1f,
        .specularStrength = 0.5f,
        .ambientStrength = 0.2f
    };
    lightCubes[1] = (Cube){ 
        .position = {0.0f, -1.5f, 0.0f}, 
        .scale = {2.0f, 2.0f, 2.0f}, 
        .rotation = {0,0,0}, 
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
        .shininess = 50.0f,
        .diffuseStrength = 1.1f,
        .specularStrength = 0.5f,
        .ambientStrength = 0.2f
    };
    lightCubes[2] = (Cube){ 
        .position = {0.0f, -1.5f, 0.0f}, 
        .scale = {2.0f, 2.0f, 2.0f}, 
        .rotation = {0,0,0}, 
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
        .shininess = 50.0f,
        .diffuseStrength = 1.1f,
        .specularStrength = 0.5f,
        .ambientStrength = 0.2f
    };

    // Floor
    cubes[0] = (Cube){ 
        .position = {0.0f, -1.5f, 0.0f}, 
        .scale = {20.0f, 0.2f, 20.0f}, 
        .rotation = {0,0,0}, 
        .color = {0.7f, 0.7f, 0.7f, 1.0f},
        .shininess = 50.0f,
        .diffuseStrength = 1.1f,
        .specularStrength = 0.5f,
        .ambientStrength = 0.2f
    };

    // Wall
    cubes[1] = (Cube){ 
        .position = {0.0f, 3.0f, -5.0f}, 
        .scale = {15.0f, 6.0f, 0.2f}, 
        .rotation = {0,0,0}, 
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
        .shininess = 50.0f,
        .diffuseStrength = 1.1f,
        .specularStrength = 0.5f,
        .ambientStrength = 0.2f
    };

    // Small cubes in a row
    for(int i = 2; i < 7; i++) {
        cubes[i].position[0] = -3.0f + (i-2)*1.5f;
        cubes[i].position[1] = -1.3f + (i-2)*1.2f;
        cubes[i].position[2] = -1.0f;
        cubes[i].scale[0] = 1.0f; cubes[i].scale[1] = 1.0f; cubes[i].scale[2] = 1.0f;
        cubes[i].rotation[0] = 0; cubes[i].rotation[1] = (i-2)*0.3f; cubes[i].rotation[2] = 0;
        cubes[i].color[0] = 1.0f; cubes[i].color[1] = 1.0f; cubes[i].color[2] = 1.0f; cubes[i].color[3] = 1.0f;
        cubes[i].shininess = 0.5f;
        cubes[i].diffuseStrength = 0.8f;
        cubes[i].specularStrength = 0.5f;
        cubes[i].ambientStrength = 0.2f;
    }

    // Extra cubes
    cubes[7] = (Cube){ 
        .position = {3.0f, -0.5f, 1.0f}, 
        .scale = {1.5f, 1.5f, 1.5f}, 
        .rotation = {0,0,0}, 
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
        .shininess = 0.5f,
        .diffuseStrength = 0.8f,
        .specularStrength = 0.4f,
        .ambientStrength = 0.2f
    };

    cubes[8] = (Cube){ 
        .position = {-3.5f, 1.0f, 2.0f}, 
        .scale = {1.2f, 1.2f, 1.2f}, 
        .rotation = {0,0.5f,0}, 
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
        .shininess = 0.5f,
        .diffuseStrength = 0.8f,
        .specularStrength = 1.4f,
        .ambientStrength = 0.2f
    };

    cubes[9] = (Cube){ 
        .position = {1.0f, 2.0f, -2.0f}, 
        .scale = {0.8f, 0.8f, 0.8f}, 
        .rotation = {0,0,0}, 
        .color = {1.0f, 1.0f, 1.0f, 1.0f},
        .shininess = 50.0f,
        .diffuseStrength = 0.8f,
        .specularStrength = 1.4f,
        .ambientStrength = 0.2f
    };

    Light lights[3];
    lights[0] = (Light){ 
        .position = {3.0f, 3.0f, 3.0f}, 
        .color = {1.1f, 1.1f, 1.1f}, 
        .ambient = 0.1f, 
        .diffuse = 0.8f, 
        .specular = 1.0f,
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    };

    lights[1] = (Light){ 
        .position = {-3.0f, 2.0f, -2.0f}, 
        .color = {0.7f, 0.7f, 0.7f}, 
        .ambient = 0.1f, 
        .diffuse = 0.6f, 
        .specular = 0.5f,
        .constant = 1.0f,
        .linear = 0.14f,
        .quadratic = 0.07f
    };

    lights[2] = (Light){ 
        .position = {0.0f, 4.0f, 0.0f}, 
        .color = {0.6f, 0.3f, 0.3f}, 
        .ambient = 0.1f, 
        .diffuse = 0.5f, 
        .specular = 0.3f,
        .constant = 1.0f,
        .linear = 0.22f,
        .quadratic = 0.20f
    };
    glUseProgram(ssaoShaderProgram);
    for(int i = 0; i < 64; ++i) {
        char name[32];
        sprintf(name, "samples[%d]", i);
        glUniform3fv(glGetUniformLocation(ssaoShaderProgram, name), 1, ssaoKernel[i]);
    }
    glUniform1i(glGetUniformLocation(ssaoShaderProgram, "gPosition"), 0);
    glUniform1i(glGetUniformLocation(ssaoShaderProgram, "gNormal"), 1);
    glUniform1i(glGetUniformLocation(ssaoShaderProgram, "texNoise"), 2);

    float farPlane =25.0f;
    
    // Post-processing parameters
    float exposure = 0.7f;
    float bloomThreshold = 1.25f;
    float bloomStrength = 0.5f;
    int blurAmount = 18;
    bool useBloom = true;
    bool useSSAO = true;

    // Main render loop
    while(!glfwWindowShouldClose(window)) {
        CAMERA_loadFromFile(camera, "camera.txt");
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Animate first light
        lights[0].position[0] = sin(currentFrame * 0.7f) * 2.0f;
        lights[0].position[2] = cos(currentFrame * 0.7f) * 2.0f;
        mat4 view, projection;
        CAMERA_perspective(camera,screenX, screenY,view,projection);
        //Shadow pass
        for(int lightIdx = 0; lightIdx < sizeof(lights)/sizeof(Light); lightIdx++) {SHADOWS_shadowPassSetup(lights, lightIdx, farPlane);
            renderScene(shadowShaderProgram, cubeVAO, cubes, 10);
        }
        glViewport(0, 0, screenX, screenY);

        // ============ FORWARD RENDERING PASS (to HDR MSAA buffer) ============
        glBindFramebuffer(GL_FRAMEBUFFER, hdrMsaaFBO);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(forwardShaderProgram);
        
        glUniformMatrix4fv(glGetUniformLocation(forwardShaderProgram, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(forwardShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
        glUniform1i(glGetUniformLocation(forwardShaderProgram, "numLights"), sizeof(lights)/sizeof(Light));
        glUniform1f(glGetUniformLocation(forwardShaderProgram, "farPlane"), farPlane);
        glUniform1i(glGetUniformLocation(forwardShaderProgram, "shadowMapCount"), sizeof(lights)/sizeof(Light));
        glUniform1f(glGetUniformLocation(forwardShaderProgram, "shadowDarkness"), 1.3f);
        SHADOWS_dynamicShadowMaps(lights, sizeof(lights)/sizeof(Light), &forwardShaderProgram);
        LIGHTING_sendLightsToShader(&forwardShaderProgram,lights, sizeof(lights)/sizeof(Light));
        glBindVertexArray(cubeVAO);
        for(int i = 0; i < 10; i++) {
            mat4 model;
            glm_mat4_identity(model);
            glm_translate(model, cubes[i].position);
            glm_rotate_y(model, cubes[i].rotation[1], model);
            glm_scale(model, cubes[i].scale);
            glUniformMatrix4fv(glGetUniformLocation(forwardShaderProgram, "model"), 1, GL_FALSE, (float*)model);
            glUniform4fv(glGetUniformLocation(forwardShaderProgram, "objectColor"), 1, cubes[i].color);
            glUniform4fv(glGetUniformLocation(forwardShaderProgram, "material.color"), 1, cubes[i].color);
            glUniform1f(glGetUniformLocation(forwardShaderProgram, "material.ambient"), cubes[i].ambientStrength);
            glUniform1f(glGetUniformLocation(forwardShaderProgram, "material.diffuse"), cubes[i].diffuseStrength);
            glUniform1f(glGetUniformLocation(forwardShaderProgram, "material.specular"), cubes[i].specularStrength);
            glUniform1f(glGetUniformLocation(forwardShaderProgram, "material.shininess"), cubes[i].shininess);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
                // Now render the light cubes (unlit, bright) into the SAME buffer
        glUseProgram(unlitShaderProgram);
        glBindVertexArray(cubeVAO);
        glUniformMatrix4fv(glGetUniformLocation(unlitShaderProgram, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(unlitShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);

        for(int i = 0; i < sizeof(lights)/sizeof(Light); i++) {
            mat4 model;
            glm_mat4_identity(model);
            glm_translate(model, lights[i].position);
            glm_scale(model, (vec3){0.3f, 0.3f, 0.3f});
            glUniformMatrix4fv(glGetUniformLocation(unlitShaderProgram, "model"), 1, GL_FALSE, (float*)model);
            
            vec4 emissiveColor = {lights[i].color[0] * 6.0f, lights[i].color[1] * 6.0f, lights[i].color[2] * 6.0f, 1.0f};
            glUniform4fv(glGetUniformLocation(unlitShaderProgram, "objectColor"), 1, emissiveColor);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        // ============ RESOLVE MSAA TO POST-PROCESSING BUFFER ============
        glBindFramebuffer(GL_READ_FRAMEBUFFER, hdrMsaaFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, postProcFBO);
        glBlitFramebuffer(0, 0, screenX, screenY, 0, 0, screenX, screenY, 
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);

        // ============ BLOOM: EXTRACT BRIGHT AREAS ============
        glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[0]);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(brightnessShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcColorBuffer);
        glUniform1i(glGetUniformLocation(brightnessShaderProgram, "hdrBuffer"), 0);
        glUniform1f(glGetUniformLocation(brightnessShaderProgram, "threshold"), bloomThreshold);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ============ BLOOM: GAUSSIAN BLUR (PING-PONG) ============
        bool horizontal = true;
        glUseProgram(gaussianBlurShaderProgram);
        for(int i = 0; i < blurAmount; i++) {
            glBindFramebuffer(GL_FRAMEBUFFER, bloomFBO[horizontal]);
            glUniform1i(glGetUniformLocation(gaussianBlurShaderProgram, "horizontal"), horizontal);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, bloomColorBuffers[!horizontal]);
            glUniform1i(glGetUniformLocation(gaussianBlurShaderProgram, "image"), 0);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            horizontal = !horizontal;
        }

        // ============ SSAO: GEOMETRY PASS ============
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoDepthFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(depthShaderProgram);
        glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(depthShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
        renderScene(depthShaderProgram, cubeVAO, cubes, 10);

        // ============ SSAO: COMPUTE OCCLUSION ============
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(ssaoShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);
        glUniformMatrix4fv(glGetUniformLocation(ssaoShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
        glUniform2f(glGetUniformLocation(ssaoShaderProgram, "screenSize"), (float)screenX, (float)screenY);
        glBindVertexArray(quadVAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ============ SSAO: BLUR ============
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(blurShaderProgram);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        glUniform1i(glGetUniformLocation(blurShaderProgram, "ssaoInput"), 0);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // ============ FINAL POST-PROCESSING: HDR + BLOOM + SSAO ============
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(postProcShaderProgram);
        
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, postProcColorBuffer);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, bloomColorBuffers[!horizontal]);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

        glUniform1i(glGetUniformLocation(postProcShaderProgram, "hdrBuffer"), 0);
        glUniform1i(glGetUniformLocation(postProcShaderProgram, "bloomBlur"), 1);
        glUniform1i(glGetUniformLocation(postProcShaderProgram, "ssaoColor"), 2);
        glUniform1f(glGetUniformLocation(postProcShaderProgram, "exposure"), exposure);
        glUniform1f(glGetUniformLocation(postProcShaderProgram, "bloomStrength"), bloomStrength);
        glUniform1i(glGetUniformLocation(postProcShaderProgram, "useBloom"), useBloom);
        glUniform1i(glGetUniformLocation(postProcShaderProgram, "useSSAO"), useSSAO);

        glUniform1f(glGetUniformLocation(postProcShaderProgram, "gamma"), 2.6f);
        glUniform1i(glGetUniformLocation(postProcShaderProgram, "toneMapMode"), 1);

        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteFramebuffers(1, &hdrMsaaFBO);
    glDeleteFramebuffers(1, &postProcFBO);
    glDeleteFramebuffers(2, bloomFBO);
    glDeleteTextures(1, &hdrMsaaColorBuffer);
    glDeleteTextures(1, &postProcColorBuffer);
    glDeleteTextures(2, bloomColorBuffers);
    glDeleteRenderbuffers(1, &hdrMsaaRboDepth);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteFramebuffers(1, &ssaoDepthFBO);
    glDeleteFramebuffers(1, &ssaoFBO);
    glDeleteFramebuffers(1, &ssaoBlurFBO);
    glDeleteFramebuffers(3, depthMapFBO);
    glDeleteTextures(1, &gPosition);
    glDeleteTextures(1, &gNormal);
    glDeleteTextures(1, &ssaoColorBuffer);
    glDeleteTextures(1, &ssaoColorBufferBlur);
    glDeleteTextures(1, &noiseTexture);
    glDeleteTextures(3, depthCubemaps);
    glDeleteRenderbuffers(1, &rboDepth);
    glDeleteProgram(forwardShaderProgram);
    glDeleteProgram(depthShaderProgram);
    glDeleteProgram(ssaoShaderProgram);
    glDeleteProgram(blurShaderProgram);
    glDeleteProgram(brightnessShaderProgram);
    glDeleteProgram(gaussianBlurShaderProgram);
    glDeleteProgram(postProcShaderProgram);
    glDeleteProgram(shadowShaderProgram);
    
    free(camera);
    glfwTerminate();
    return 0;
}