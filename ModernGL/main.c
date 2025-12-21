#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include "cglm/cglm.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
 glViewport(0, 0, width, height);
}

// Camera globals
vec3 cameraPos = {0.0f, 2.0f, 8.0f};
vec3 cameraFront = {0.0f, 0.0f, -1.0f};
vec3 cameraUp = {0.0f, 1.0f, 0.0f};

float lastX = 400, lastY = 300;
float yaw = -90.0f;
float pitch = 0.0f;
int firstMouse = 1;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
 if (firstMouse) {
 lastX = xpos;
 lastY = ypos;
 firstMouse = 0;
 }

 float xoffset = xpos - lastX;
 float yoffset = lastY - ypos;
 lastX = xpos;
 lastY = ypos;

 float sensitivity = 0.1f;
 xoffset *= sensitivity;
 yoffset *= sensitivity;

 yaw += xoffset;
 pitch += yoffset;

 if (pitch > 89.0f) pitch = 89.0f;
 if (pitch < -89.0f) pitch = -89.0f;

 cameraFront[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
 cameraFront[1] = sin(glm_rad(pitch));
 cameraFront[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
 glm_vec3_normalize(cameraFront);
}

void processInput(GLFWwindow* window) {
 float cameraSpeed = 2.5f * deltaTime;
 vec3 temp;
 if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
 glm_vec3_scale(cameraFront, cameraSpeed, temp);
 glm_vec3_add(cameraPos, temp, cameraPos);
 }
 if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
 glm_vec3_scale(cameraFront, cameraSpeed, temp);
 glm_vec3_sub(cameraPos, temp, cameraPos);
 }
 if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
 vec3 crossVec;
 glm_vec3_cross(cameraFront, cameraUp, crossVec);
 glm_vec3_normalize(crossVec);
 glm_vec3_scale(crossVec, cameraSpeed, temp);
 glm_vec3_sub(cameraPos, temp, cameraPos);
 }
 if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
 vec3 crossVec;
 glm_vec3_cross(cameraFront, cameraUp, crossVec);
 glm_vec3_normalize(crossVec);
 glm_vec3_scale(crossVec, cameraSpeed, temp);
 glm_vec3_add(cameraPos, temp, cameraPos);
 }
}

float vertices[] = {
 // positions // normals
 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f,
 -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f,

 -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
 -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,

 -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f,
 -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f,

 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f,
 0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f,

 -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,
 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
 0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
 -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f,
 -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f,

 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
 0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
 0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
 -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f,
 -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f
};

float quadVertices[] = {
 -1.0f, 1.0f, 0.0f, 1.0f,
 -1.0f, -1.0f, 0.0f, 0.0f,
 1.0f, -1.0f, 1.0f, 0.0f,
 -1.0f, 1.0f, 0.0f, 1.0f,
 1.0f, -1.0f, 1.0f, 0.0f,
 1.0f, 1.0f, 1.0f, 1.0f
};

// Geometry pass - outputs position and normal in view space
const char *geoVertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"layout(location=1) in vec3 aNormal;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"out vec3 FragPos;\n"
"out vec3 Normal;\n"
"out vec3 ViewPos;\n"
"void main(){\n"
" vec4 viewPos = view * model * vec4(aPos, 1.0);\n"
" ViewPos = viewPos.xyz;\n"
" gl_Position = projection * viewPos;\n"
" FragPos = vec3(model * vec4(aPos, 1.0));\n"
" Normal = mat3(transpose(inverse(view * model))) * aNormal;\n"
"}\0";

const char *geoFragmentShaderSource = "#version 330 core\n"
"layout(location=0) out vec4 gPosition;\n"
"layout(location=1) out vec4 gNormal;\n"
"layout(location=2) out vec4 gColor;\n"
"in vec3 FragPos;\n"
"in vec3 Normal;\n"
"in vec3 ViewPos;\n"
"uniform vec3 viewPos;\n"
"uniform vec4 objectColor;\n"
"struct Light{\n"
" vec3 position;\n"
" vec3 color;\n"
" float ambient;\n"
" float diffuse;\n"
" float specular;\n"
"};\n"
"uniform Light lights[4];\n"
"uniform int numLights;\n"
"void main(){\n"
" gPosition = vec4(ViewPos, 1.0);\n"
" gNormal = vec4(normalize(Normal), 1.0);\n"
" vec3 norm = normalize(Normal);\n"
" vec3 viewDir = normalize(-ViewPos);\n"
" vec3 result = vec3(0.0);\n"
" for(int i=0; i<numLights; i++){\n"
" vec3 ambient = lights[i].ambient * lights[i].color;\n"
" vec3 lightDir = normalize(lights[i]. position - FragPos);\n"
" float diff = max(dot(norm, lightDir), 0.0);\n"
" vec3 diffuse = lights[i].diffuse * diff * lights[i].color;\n"
" vec3 halfwayDir = normalize(lightDir + viewDir);\n"
" float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);\n"
" vec3 specular = lights[i].specular * spec * lights[i].color;\n"
" result += ambient + diffuse + specular;\n"
" }\n"
" gColor = vec4(result * objectColor.rgb, 1.0);\n"
"}\0";

// SSAO shader - samples hemisphere around fragment
const char *ssaoVertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec2 aPos;\n"
"layout(location=1) in vec2 aTexCoords;\n"
"out vec2 TexCoords;\n"
"void main(){\n"
" TexCoords = aTexCoords;\n"
" gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}\0";

const char *ssaoFragmentShaderSource = "#version 330 core\n"
"out float FragColor;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D gPosition;\n"
"uniform sampler2D gNormal;\n"
"uniform sampler2D texNoise;\n"
"uniform vec3 samples[64];\n"
"uniform mat4 projection;\n"
"uniform vec2 screenSize;\n"
"void main(){\n"
" vec3 fragPos = texture(gPosition, TexCoords).xyz;\n"
" vec3 normal = normalize(texture(gNormal, TexCoords).rgb);\n"
" if(length(normal) < 0.1){\n"
" FragColor = 1.0;\n"
" return;\n"
" }\n"
" vec2 noiseScale = screenSize / 4.0;\n"
" vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);\n"
" vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));\n"
" vec3 bitangent = cross(normal, tangent);\n"
" mat3 TBN = mat3(tangent, bitangent, normal);\n"
" float occlusion = 0.0;\n"
" float radius = 0.5;\n"
" float bias = 0.025;\n"
" for(int i = 0; i < 64; ++i){\n"
" vec3 samplePos = TBN * samples[i];\n"
" samplePos = fragPos + samplePos * radius;\n"
" vec4 offset = vec4(samplePos, 1.0);\n"
" offset = projection * offset;\n"
" offset.xyz /= offset.w;\n"
" offset.xyz = offset.xyz * 0.5 + 0.5;\n"
" float sampleDepth = texture(gPosition, offset.xy).z;\n"
" float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));\n"
" occlusion += (sampleDepth >= samplePos.z + bias ?  1.0 : 0.0) * rangeCheck;\n"
" }\n"
" occlusion = 1.0 - (occlusion / 64.0);\n"
" FragColor = occlusion;\n"
"}\0";

// Blur shader
const char *blurFragmentShaderSource = "#version 330 core\n"
"out float FragColor;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D ssaoInput;\n"
"void main(){\n"
" vec2 texelSize = 1.0 / vec2(textureSize(ssaoInput, 0));\n"
" float result = 0.0;\n"
" for (int x = -2; x < 2; ++x){\n"
" for (int y = -2; y < 2; ++y){\n"
" vec2 offset = vec2(float(x), float(y)) * texelSize;\n"
" result += texture(ssaoInput, TexCoords + offset).r;\n"
" }\n"
" }\n"
" FragColor = result / 16.0;\n"
"}\0";

// Final composite shader - blends SSAO with base color
const char *finalVertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec2 aPos;\n"
"layout(location=1) in vec2 aTexCoords;\n"
"out vec2 TexCoords;\n"
"void main(){\n"
" TexCoords = aTexCoords;\n"
" gl_Position = vec4(aPos, 0.0, 1.0);\n"
"}\0";

const char *finalFragmentShaderSource = "#version 330 core\n"
"out vec4 FragColor;\n"
"in vec2 TexCoords;\n"
"uniform sampler2D ssaoInput;\n"
"uniform sampler2D gColor;\n"
"void main(){\n"
" vec3 color = texture(gColor, TexCoords).rgb;\n"
" float ssao = texture(ssaoInput, TexCoords).r;\n"
" vec3 result = color * ssao;\n"
" FragColor = vec4(result, 1.0);\n"
"}\0";

// Light source shader
const char *lightVertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec3 aPos;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 projection;\n"
"void main(){gl_Position=projection*view*model*vec4(aPos,1.0);}\0";

const char *lightFragmentShaderSource = "#version 330 core\n"
"layout(location=0) out vec4 gPosition;\n"
"layout(location=1) out vec4 gNormal;\n"
"layout(location=2) out vec4 gColor;\n"
"uniform vec3 lightColor;\n"
"void main(){\n"
" gPosition = vec4(0.0);\n"
" gNormal = vec4(0.0);\n"
" gColor = vec4(lightColor, 1.0);\n"
"}\0";

typedef struct {
 vec3 position;
 vec3 scale;
 vec3 rotation;
 vec4 color;
} Cube;

typedef struct {
 vec3 position;
 vec3 color;
 float ambient;
 float diffuse;
 float specular;
} Light;

float lerp(float a, float b, float f) {
 return a + f * (b - a);
}

int main() {
 srand(time(NULL));
 glfwInit();
 glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
 glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
 glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

 GLFWwindow* window = glfwCreateWindow(800, 600, "SSAO Demo", NULL, NULL);
 if(! window){ printf("Failed\n"); glfwTerminate(); return -1; }
 glfwMakeContextCurrent(window);
 if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){ printf("Failed GLAD\n"); return -1; }

 glViewport(0, 0, 800, 600);
 glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
 glfwSetCursorPosCallback(window, mouse_callback);
 glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

 glEnable(GL_DEPTH_TEST);

 // Compile shaders
 unsigned int geoVertexShader = glCreateShader(GL_VERTEX_SHADER);
 glShaderSource(geoVertexShader, 1, &geoVertexShaderSource, NULL);
 glCompileShader(geoVertexShader);
 unsigned int geoFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource(geoFragmentShader, 1, &geoFragmentShaderSource, NULL);
 glCompileShader(geoFragmentShader);
 unsigned int geoShaderProgram = glCreateProgram();
 glAttachShader(geoShaderProgram, geoVertexShader);
 glAttachShader(geoShaderProgram, geoFragmentShader);
 glLinkProgram(geoShaderProgram);
 glDeleteShader(geoVertexShader);
 glDeleteShader(geoFragmentShader);

 unsigned int ssaoVertexShader = glCreateShader(GL_VERTEX_SHADER);
 glShaderSource(ssaoVertexShader, 1, &ssaoVertexShaderSource, NULL);
 glCompileShader(ssaoVertexShader);
 unsigned int ssaoFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource(ssaoFragmentShader, 1, &ssaoFragmentShaderSource, NULL);
 glCompileShader(ssaoFragmentShader);
 unsigned int ssaoShaderProgram = glCreateProgram();
 glAttachShader(ssaoShaderProgram, ssaoVertexShader);
 glAttachShader(ssaoShaderProgram, ssaoFragmentShader);
 glLinkProgram(ssaoShaderProgram);
 glDeleteShader(ssaoFragmentShader);

 unsigned int blurFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource(blurFragmentShader, 1, &blurFragmentShaderSource, NULL);
 glCompileShader(blurFragmentShader);
 unsigned int blurShaderProgram = glCreateProgram();
 glAttachShader(blurShaderProgram, ssaoVertexShader);
 glAttachShader(blurShaderProgram, blurFragmentShader);
 glLinkProgram(blurShaderProgram);
 glDeleteShader(blurFragmentShader);

 unsigned int finalVertexShader = glCreateShader(GL_VERTEX_SHADER);
 glShaderSource(finalVertexShader, 1, &finalVertexShaderSource, NULL);
 glCompileShader(finalVertexShader);
 unsigned int finalFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource(finalFragmentShader, 1, &finalFragmentShaderSource, NULL);
 glCompileShader(finalFragmentShader);
 unsigned int finalShaderProgram = glCreateProgram();
 glAttachShader(finalShaderProgram, finalVertexShader);
 glAttachShader(finalShaderProgram, finalFragmentShader);
 glLinkProgram(finalShaderProgram);
 glDeleteShader(finalVertexShader);
 glDeleteShader(finalFragmentShader);

 unsigned int lightVertexShader = glCreateShader(GL_VERTEX_SHADER);
 glShaderSource(lightVertexShader, 1, &lightVertexShaderSource, NULL);
 glCompileShader(lightVertexShader);
 unsigned int lightFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
 glShaderSource(lightFragmentShader, 1, &lightFragmentShaderSource, NULL);
 glCompileShader(lightFragmentShader);
 unsigned int lightShaderProgram = glCreateProgram();
 glAttachShader(lightShaderProgram, lightVertexShader);
 glAttachShader(lightShaderProgram, lightFragmentShader);
 glLinkProgram(lightShaderProgram);
 glDeleteShader(lightVertexShader);
 glDeleteShader(lightFragmentShader);

 // Create G-buffer
 unsigned int gBuffer;
 glGenFramebuffers(1, &gBuffer);
 glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

 unsigned int gPosition, gNormal, gColor;
 glGenTextures(1, &gPosition);
 glBindTexture(GL_TEXTURE_2D, gPosition);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

 glGenTextures(1, &gNormal);
 glBindTexture(GL_TEXTURE_2D, gNormal);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 800, 600, 0, GL_RGBA, GL_FLOAT, NULL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

 glGenTextures(1, &gColor);
 glBindTexture(GL_TEXTURE_2D, gColor);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 800, 600, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gColor, 0);

 unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
 glDrawBuffers(3, attachments);

 unsigned int rboDepth;
 glGenRenderbuffers(1, &rboDepth);
 glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
 glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 800, 600);
 glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);

 if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
 printf("Framebuffer not complete!\n");

 // Create SSAO framebuffer
 unsigned int ssaoFBO, ssaoBlurFBO;
 glGenFramebuffers(1, &ssaoFBO);
 glGenFramebuffers(1, &ssaoBlurFBO);
 glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

 unsigned int ssaoColorBuffer, ssaoColorBufferBlur;
 glGenTextures(1, &ssaoColorBuffer);
 glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 800, 600, 0, GL_RED, GL_FLOAT, NULL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);

 glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
 glGenTextures(1, &ssaoColorBufferBlur);
 glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
 glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 800, 600, 0, GL_RED, GL_FLOAT, NULL);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
 glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
 glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);

 glBindFramebuffer(GL_FRAMEBUFFER, 0);

 // Generate sample kernel
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

 // Create scene
 Cube cubes[10];
 cubes[0] = (Cube){
 .position = {0.0f, -1.5f, 0.0f},
 .scale = {20.0f, 0.2f, 20.0f},
 .rotation = {0,0,0},
 .color = {0.7f, 0.7f, 0.7f, 1.0f}
 };

 cubes[1] = (Cube){
 .position = {0.0f, 3.0f, -5.0f},
 .scale = {15.0f, 6.0f, 0.2f},
 .rotation = {0,0,0},
 .color = {0.6f, 0.6f, 0.6f, 1.0f}
 };

 for(int i = 2; i < 7; i++) {
 cubes[i]. position[0] = -3.0f + (i-2)*1.5f;
 cubes[i].position[1] = -1.3f + (i-2)*1.2f;
 cubes[i].position[2] = -1.0f;
 cubes[i].scale[0] = 1.0f;
 cubes[i].scale[1] = 1.0f;
 cubes[i].scale[2] = 1.0f;
 cubes[i].rotation[0] = 0;
 cubes[i].rotation[1] = (i-2)*0.3f;
 cubes[i].rotation[2] = 0;
 cubes[i].color[0] = 0.8f;
 cubes[i].color[1] = 0.8f;
 cubes[i].color[2] = 0.8f;
 cubes[i].color[3] = 1.0f;
 }

 cubes[7] = (Cube){
 .position = {3.0f, -0.5f, 1.0f},
 .scale = {1.5f, 1.5f, 1.5f},
 .rotation = {0,0,0},
 .color = {0.75f, 0.75f, 0.75f, 1.0f}
 };

 cubes[8] = (Cube){
 .position = {-3.5f, 1.0f, 2.0f},
 .scale = {1.2f, 1.2f, 1.2f},
 .rotation = {0,0.5f,0},
 .color = {0.75f, 0.75f, 0.75f, 1.0f}
 };

 cubes[9] = (Cube){
 .position = {1.0f, 2.0f, -2.0f},
 . scale = {0.8f, 0.8f, 0.8f},
 .rotation = {0,0,0},
 .color = {0.7f, 0.7f, 0.7f, 1.0f}
 };

 Light lights[3];
 lights[0]. position[0] = 3.0f; lights[0].position[1] = 3.0f; lights[0]. position[2] = 3.0f;
 lights[0].color[0] = 0.9f; lights[0].color[1] = 0.9f; lights[0].color[2] = 0.9f;
 lights[0].ambient = 0.15f; lights[0].diffuse = 0.8f; lights[0].specular = 1.0f;

 lights[1].position[0] = -3.0f; lights[1].position[1] = 2.0f; lights[1]. position[2] = -2.0f;
 lights[1].color[0] = 0.7f; lights[1].color[1] = 0.7f; lights[1].color[2] = 0.7f;
 lights[1].ambient = 0.1f; lights[1].diffuse = 0.6f; lights[1].specular = 0.5f;

 lights[2]. position[0] = 0.0f; lights[2]. position[1] = 4.0f; lights[2]. position[2] = 0.0f;
 lights[2].color[0] = 0.6f; lights[2].color[1] = 0.6f; lights[2].color[2] = 0.6f;
 lights[2].ambient = 0.1f; lights[2].diffuse = 0.5f; lights[2].specular = 0.3f;

 // Send samples to shader
 glUseProgram(ssaoShaderProgram);
 for(int i = 0; i < 64; ++i) {
 char name[32];
 sprintf(name, "samples[%d]", i);
 glUniform3fv(glGetUniformLocation(ssaoShaderProgram, name), 1, ssaoKernel[i]);
 }
 glUniform1i(glGetUniformLocation(ssaoShaderProgram, "gPosition"), 0);
 glUniform1i(glGetUniformLocation(ssaoShaderProgram, "gNormal"), 1);
 glUniform1i(glGetUniformLocation(ssaoShaderProgram, "texNoise"), 3);

 while(!glfwWindowShouldClose(window)){
 float currentFrame = glfwGetTime();
 deltaTime = currentFrame - lastFrame;
 lastFrame = currentFrame;

 processInput(window);

 lights[0].position[0] = sin(currentFrame * 0.7f) * 4.0f;
 lights[0].position[2] = cos(currentFrame * 0.7f) * 4.0f;

 // 1. Geometry Pass
 glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
 glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

 mat4 view, projection;
 vec3 target = {cameraPos[0]+cameraFront[0], cameraPos[1]+cameraFront[1], cameraPos[2]+cameraFront[2]};
 glm_lookat(cameraPos, target, cameraUp, view);
 glm_perspective(glm_rad(45.0f), 800.0f/600.0f, 0.1f, 100.0f, projection);

 glUseProgram(geoShaderProgram);
 glBindVertexArray(cubeVAO);
 glUniformMatrix4fv(glGetUniformLocation(geoShaderProgram, "view"), 1, GL_FALSE, (float*)view);
 glUniformMatrix4fv(glGetUniformLocation(geoShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
 glUniform3fv(glGetUniformLocation(geoShaderProgram, "viewPos"), 1, cameraPos);
 glUniform1i(glGetUniformLocation(geoShaderProgram, "numLights"), 3);

 for(int i=0; i<3; i++){
 char name[50];
 sprintf(name, "lights[%d]. position", i);
 glUniform3fv(glGetUniformLocation(geoShaderProgram, name), 1, lights[i].position);
 sprintf(name, "lights[%d].color", i);
 glUniform3fv(glGetUniformLocation(geoShaderProgram, name), 1, lights[i].color);
 sprintf(name, "lights[%d].ambient", i);
 glUniform1f(glGetUniformLocation(geoShaderProgram, name), lights[i].ambient);
 sprintf(name, "lights[%d].diffuse", i);
 glUniform1f(glGetUniformLocation(geoShaderProgram, name), lights[i].diffuse);
 sprintf(name, "lights[%d]. specular", i);
 glUniform1f(glGetUniformLocation(geoShaderProgram, name), lights[i].specular);
 }

 for(int i=0; i<10; i++){
 mat4 model;
 glm_mat4_identity(model);
 glm_translate(model, cubes[i]. position);
 glm_rotate_y(model, cubes[i].rotation[1], model);
 glm_scale(model, cubes[i]. scale);
 glUniformMatrix4fv(glGetUniformLocation(geoShaderProgram, "model"), 1, GL_FALSE, (float*)model);
 glUniform4fv(glGetUniformLocation(geoShaderProgram, "objectColor"), 1, cubes[i].color);
 glDrawArrays(GL_TRIANGLES, 0, 36);
 }

 glUseProgram(lightShaderProgram);
 glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, (float*)view);
 glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
 for(int i=0; i<3; i++){
 mat4 model;
 glm_mat4_identity(model);
 glm_translate(model, lights[i].position);
 glm_scale(model, (vec3){0.15f, 0.15f, 0.15f});
 glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, (float*)model);
 glUniform3fv(glGetUniformLocation(lightShaderProgram, "lightColor"), 1, lights[i].color);
 glDrawArrays(GL_TRIANGLES, 0, 36);
 }

 // 2. SSAO Pass
 glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
 glClear(GL_COLOR_BUFFER_BIT);
 glUseProgram(ssaoShaderProgram);
 glActiveTexture(GL_TEXTURE0);
 glBindTexture(GL_TEXTURE_2D, gPosition);
 glActiveTexture(GL_TEXTURE1);
 glBindTexture(GL_TEXTURE_2D, gNormal);
 glActiveTexture(GL_TEXTURE3);
 glBindTexture(GL_TEXTURE_2D, noiseTexture);
 glUniformMatrix4fv(glGetUniformLocation(ssaoShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
 glUniform2f(glGetUniformLocation(ssaoShaderProgram, "screenSize"), 800.0f, 600.0f);
 glBindVertexArray(quadVAO);
 glDrawArrays(GL_TRIANGLES, 0, 6);

 // 3. Blur Pass
 glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
 glClear(GL_COLOR_BUFFER_BIT);
 glUseProgram(blurShaderProgram);
 glActiveTexture(GL_TEXTURE0);
 glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
 glUniform1i(glGetUniformLocation(blurShaderProgram, "ssaoInput"), 0);
 glDrawArrays(GL_TRIANGLES, 0, 6);

 // 4. Final Pass - composite SSAO with color
 glBindFramebuffer(GL_FRAMEBUFFER, 0);
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 glUseProgram(finalShaderProgram);
 glActiveTexture(GL_TEXTURE0);
 glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
 glUniform1i(glGetUniformLocation(finalShaderProgram, "ssaoInput"), 0);
 glActiveTexture(GL_TEXTURE1);
 glBindTexture(GL_TEXTURE_2D, gColor);
 glUniform1i(glGetUniformLocation(finalShaderProgram, "gColor"), 1);
 glBindVertexArray(quadVAO);
 glDrawArrays(GL_TRIANGLES, 0, 6);

 glfwSwapBuffers(window);
 glfwPollEvents();
 }

 glDeleteVertexArrays(1, &cubeVAO);
 glDeleteVertexArrays(1, &quadVAO);
 glDeleteBuffers(1, &cubeVBO);
 glDeleteBuffers(1, &quadVBO);
 glDeleteFramebuffers(1, &gBuffer);
 glDeleteFramebuffers(1, &ssaoFBO);
 glDeleteFramebuffers(1, &ssaoBlurFBO);
 glDeleteTextures(1, &gPosition);
 glDeleteTextures(1, &gNormal);
 glDeleteTextures(1, &gColor);
 glDeleteTextures(1, &ssaoColorBuffer);
 glDeleteTextures(1, &ssaoColorBufferBlur);
 glDeleteTextures(1, &noiseTexture);
 glDeleteRenderbuffers(1, &rboDepth);
 glDeleteProgram(geoShaderProgram);
 glDeleteProgram(ssaoShaderProgram);
 glDeleteProgram(blurShaderProgram);
 glDeleteProgram(finalShaderProgram);
 glDeleteProgram(lightShaderProgram);
 glfwTerminate();
 return 0;
}