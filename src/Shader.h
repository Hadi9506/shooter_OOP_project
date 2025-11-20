#pragma once
#include <string>
#include <glm/glm.hpp>

class Shader {
public:
    unsigned int ID;
    Shader(const char* vertexPath, const char* fragmentPath);
    void use() const;
    void setMat4(const std::string& name, const glm::mat4& mat) const;
private:
    std::string readFile(const char* path);
    void checkCompileErrors(unsigned int shader, const std::string& type);
};