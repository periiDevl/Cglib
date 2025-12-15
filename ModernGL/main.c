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
vec3 cameraPos = {0.0f, 0.0f, 5.0f};
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

// Cube vertices - each face has its own vertices with proper normals
float vertices[] = {
	// Back face (z = -0.5)
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	// Front face (z = 0.5)
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

	// Left face (x = -0.5)
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	// Right face (x = 0.5)
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	// Bottom face (y = -0.5)
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	// Top face (y = 0.5)
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};

// Vertex shader with Blinn-Phong lighting and multiple lights
const char *vertexShaderSource = "#version 330 core\n"
	"layout(location=0) in vec3 aPos;\n"
	"layout(location=1) in vec3 aNormal;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"
	"out vec3 FragPos;\n"
	"out vec3 Normal;\n"
	"void main(){\n"
	"	gl_Position=projection*view*model*vec4(aPos,1.0);\n"
	"	FragPos=vec3(model*vec4(aPos,1.0));\n"
	"	Normal=mat3(transpose(inverse(model)))*aNormal;\n"
	"}\0";

// Fragment shader with Blinn-Phong and multiple lights
const char *fragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"in vec3 FragPos;\n"
	"in vec3 Normal;\n"
	"uniform vec3 viewPos;\n"
	"uniform vec4 objectColor;\n"
	"struct Light{\n"
	"	vec3 position;\n"
	"	vec3 color;\n"
	"	float ambient;\n"
	"	float diffuse;\n"
	"	float specular;\n"
	"};\n"
	"uniform Light lights[4];\n"
	"uniform int numLights;\n"
	"void main(){\n"
	"	vec3 norm=normalize(Normal);\n"
	"	vec3 viewDir=normalize(viewPos-FragPos);\n"
	"	vec3 result=vec3(0.0);\n"
	"	for(int i=0;i<numLights;i++){\n"
	"		vec3 ambient=lights[i].ambient*lights[i].color;\n"
	"		vec3 lightDir=normalize(lights[i].position-FragPos);\n"
	"		float diff=max(dot(norm,lightDir),0.0);\n"
	"		vec3 diffuse=lights[i].diffuse*diff*lights[i].color;\n"
	"		vec3 halfwayDir=normalize(lightDir+viewDir);\n"
	"		float spec=pow(max(dot(norm,halfwayDir),0.0),32.0);\n"
	"		vec3 specular=lights[i].specular*spec*lights[i].color;\n"
	"		result+=ambient+diffuse+specular;\n"
	"	}\n"
	"	FragColor=vec4(result*objectColor.rgb,objectColor.a);\n"
	"}\0";

// Simple light source shader
const char *lightVertexShaderSource = "#version 330 core\n"
	"layout(location=0) in vec3 aPos;\n"
	"uniform mat4 model;\n"
	"uniform mat4 view;\n"
	"uniform mat4 projection;\n"
	"void main(){gl_Position=projection*view*model*vec4(aPos,1.0);}\0";

const char *lightFragmentShaderSource = "#version 330 core\n"
	"out vec4 FragColor;\n"
	"uniform vec3 lightColor;\n"
	"void main(){FragColor=vec4(lightColor,1.0);}\0";

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

int main() {
	srand(time(NULL));
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(800, 600, "Cglib - Testing", NULL, NULL);
	if(!window) {
		printf("Failed\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		printf("Failed GLAD\n");
		return -1;
	}

	glViewport(0, 0, 800, 600);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glEnable(GL_DEPTH_TEST);

	// Compile lighting shader
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Compile light source shader
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

	unsigned int VAO, VBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// Create cubes
	Cube cubes[5];
	for(int i=0; i<5; i++) {
		cubes[i].position[0] = ((rand()%200)/100.0f - 1.0f) * 3.0f;
		cubes[i].position[1] = ((rand()%200)/100.0f - 1.0f) * 2.0f;
		cubes[i].position[2] = ((rand()%200)/100.0f - 1.0f) * 3.0f;
		cubes[i].scale[0] = ((rand()%50)/100.0f + 0.3f);
		cubes[i].scale[1] = ((rand()%50)/100.0f + 0.3f);
		cubes[i].scale[2] = ((rand()%50)/100.0f + 0.3f);
		cubes[i].rotation[0] = ((rand()%360)/180.0f)*3.14159f;
		cubes[i].rotation[1] = ((rand()%360)/180.0f)*3.14159f;
		cubes[i].rotation[2] = ((rand()%360)/180.0f)*3.14159f;
		cubes[i].color[0] = (rand()%70+30)/100.0f;
		cubes[i].color[1] = (rand()%70+30)/100.0f;
		cubes[i].color[2] = (rand()%70+30)/100.0f;
		cubes[i].color[3] = 1.0f;
	}

	// Create lights
	Light lights[4];
	float stern = 0.7f;
	lights[0].position[0] = 2.0f; lights[0].position[1] = 2.0f; lights[0].position[2] = 2.0f;
	lights[0].color[0] = stern; lights[0].color[1] = stern; lights[0].color[2] = stern;
	lights[0].ambient = 0.1f; lights[0].diffuse = 0.8f; lights[0].specular = 1.0f;

	lights[1].position[0] = -2.0f; lights[1].position[1] = 1.0f; lights[1].position[2] = -2.0f;
	lights[1].color[0] = stern; lights[1].color[1] = stern; lights[1].color[2] = stern;
	lights[1].ambient = 0.05f; lights[1].diffuse = 0.6f; lights[1].specular = 0.5f;

	lights[2].position[0] = 0.0f; lights[2].position[1] = -1.5f; lights[2].position[2] = 0.0f;
	lights[2].color[0] = stern; lights[2].color[1] = stern; lights[2].color[2] = stern;
	lights[2].ambient = 0.05f; lights[2].diffuse = 0.5f; lights[2].specular = 0.3f;

	lights[3].position[0] = 1.0f; lights[3].position[1] = 0.0f; lights[3].position[2] = -3.0f;
	lights[3].color[0] = stern; lights[3].color[1] = stern; lights[3].color[2] = stern;
	lights[3].ambient = 0.05f; lights[3].diffuse = 0.4f; lights[3].specular = 0.2f;

	while(!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		processInput(window);

		// Animate lights
		lights[0].position[0] = sin(currentFrame) * 3.0f;
		lights[0].position[2] = cos(currentFrame) * 3.0f;
		lights[1].position[1] = sin(currentFrame * 1.3f) * 2.0f;
		lights[2].position[0] = cos(currentFrame * 0.7f) * 2.5f;
		lights[2].position[2] = sin(currentFrame * 0.7f) * 2.5f;

		glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		mat4 view;
		glm_lookat(cameraPos, (vec3){cameraPos[0]+cameraFront[0], cameraPos[1]+cameraFront[1], cameraPos[2]+cameraFront[2]}, cameraUp, view);

		mat4 projection;
		glm_perspective(glm_rad(45.0f), 800.0f/600.0f, 0.1f, 100.0f, projection);

		// Draw cubes with lighting
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);

		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, (float*)view);
		glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, (float*)projection);
		glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, cameraPos);
		glUniform1i(glGetUniformLocation(shaderProgram, "numLights"), 4);

		for(int i=0; i<4; i++) {
			char uniformName[50];
			sprintf(uniformName, "lights[%d].position", i);
			glUniform3fv(glGetUniformLocation(shaderProgram, uniformName), 1, lights[i].position);
			sprintf(uniformName, "lights[%d].color", i);
			glUniform3fv(glGetUniformLocation(shaderProgram, uniformName), 1, lights[i].color);
			sprintf(uniformName, "lights[%d].ambient", i);
			glUniform1f(glGetUniformLocation(shaderProgram, uniformName), lights[i].ambient);
			sprintf(uniformName, "lights[%d].diffuse", i);
			glUniform1f(glGetUniformLocation(shaderProgram, uniformName), lights[i].diffuse);
			sprintf(uniformName, "lights[%d].specular", i);
			glUniform1f(glGetUniformLocation(shaderProgram, uniformName), lights[i].specular);
		}

		for(int i=0; i<5; i++) {
			mat4 model;
			glm_mat4_identity(model);
			glm_translate(model, cubes[i].position);
			glm_rotate_x(model, cubes[i].rotation[0], model);
			glm_rotate_y(model, cubes[i].rotation[1], model);
			glm_rotate_z(model, cubes[i].rotation[2], model);
			glm_scale(model, cubes[i].scale);

			glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, (float*)model);
			glUniform4fv(glGetUniformLocation(shaderProgram, "objectColor"), 1, cubes[i].color);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// Draw light sources
		glUseProgram(lightShaderProgram);
		glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "view"), 1, GL_FALSE, (float*)view);
		glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "projection"), 1, GL_FALSE, (float*)projection);

		for(int i=0; i<4; i++) {
			mat4 model;
			glm_mat4_identity(model);
			glm_translate(model, lights[i].position);
			glm_scale(model, (vec3){0.1f, 0.1f, 0.1f});

			glUniformMatrix4fv(glGetUniformLocation(lightShaderProgram, "model"), 1, GL_FALSE, (float*)model);
			glUniform3fv(glGetUniformLocation(lightShaderProgram, "lightColor"), 1, lights[i].color);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
	glDeleteProgram(lightShaderProgram);
	glfwTerminate();
	return 0;
}