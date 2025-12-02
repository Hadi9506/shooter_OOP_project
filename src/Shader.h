#pragma once
#include <string>
#include <glm/glm.hpp>

// Shader class manages OpenGL shader programs
// Handles compilation, linking, and uniform setting
class Shader {
public:
    unsigned int ID;  // OpenGL shader program ID
    
    // Constructor loads and compiles vertex and fragment shaders
    Shader(const char* vertexPath, const char* fragmentPath);
    
    // Activate this shader program for rendering
    void use() const;
    
    // Set a 4x4 matrix uniform in the shader
    void setMat4(const std::string& name, const glm::mat4& mat) const;
    
private:
    // Read shader source code from file
    std::string readFile(const char* path);
    
    // Check for shader compilation errors
    void checkCompileErrors(unsigned int shader, const std::string& type);
};