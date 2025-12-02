#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <sstream>
#include <iostream>

// Read shader source code from file into a string
std::string Shader::readFile(const char* path) {
    std::ifstream file(path);
    std::stringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// Constructor: load shaders from files, compile, and link them
Shader::Shader(const char* vertexPath, const char* fragmentPath) {
    // Read vertex and fragment shader source from files
    std::string vSrc = readFile(vertexPath);
    std::string fSrc = readFile(fragmentPath);
    const char* vCode = vSrc.c_str();
    const char* fCode = fSrc.c_str();

    // Compile vertex shader
    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vCode, nullptr);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // Compile fragment shader
    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fCode, nullptr);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // Link shaders into program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM");

    // Clean up shader objects after linking
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

// Activate this shader program for use
void Shader::use() const { 
    glUseProgram(ID); 
}

// Set a 4x4 matrix uniform in the shader
void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

// Check for shader compilation or linking errors
void Shader::checkCompileErrors(unsigned int shader, const std::string& type) {
    int success;
    char infoLog[1024];
    
    // Check shader compilation (not program linking)
    if (type != "PROGRAM") {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Shader compile error (" << type << "):\n" << infoLog << std::endl;
        }
    } 
    // Check program linking
    else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "Program link error:\n" << infoLog << std::endl;
        }
    }
}