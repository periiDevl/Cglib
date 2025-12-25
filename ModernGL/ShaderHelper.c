#include"ShaderHelper.h"

// Compile a standard vertex + fragment shader program
unsigned int compileShaderProgram(const char* vertexSource, const char* fragmentSource) {
    int success;
    char infoLog[512];
    
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("ERROR: Vertex Shader Compilation Failed:\n%s\n", infoLog);
    }
    
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("ERROR: Fragment Shader Compilation Failed:\n%s\n", infoLog);
    }
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR: Shader Program Linking Failed:\n%s\n", infoLog);
    }
    
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    
    return program;
}

// Compile a shader program with geometry shader
unsigned int compileShaderProgramWithGeometry(const char* vertexSource, const char* fragmentSource, const char* geometrySource) {
    int success;
    char infoLog[512];
    
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexSource, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        printf("ERROR: Vertex Shader Compilation Failed:\n%s\n", infoLog);
    }
    
    unsigned int geometry = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(geometry, 1, &geometrySource, NULL);
    glCompileShader(geometry);
    glGetShaderiv(geometry, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(geometry, 512, NULL, infoLog);
        printf("ERROR: Geometry Shader Compilation Failed:\n%s\n", infoLog);
    }
    
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentSource, NULL);
    glCompileShader(fragment);
    glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(fragment, 512, NULL, infoLog);
        printf("ERROR: Fragment Shader Compilation Failed:\n%s\n", infoLog);
    }
    
    unsigned int program = glCreateProgram();
    glAttachShader(program, vertex);
    glAttachShader(program, geometry);
    glAttachShader(program, fragment);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        printf("ERROR: Shader Program Linking Failed:\n%s\n", infoLog);
    }
    
    glDeleteShader(vertex);
    glDeleteShader(geometry);
    glDeleteShader(fragment);
    
    return program;
}