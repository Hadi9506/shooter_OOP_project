#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

class TextRenderer {
private:
    unsigned int VAO, VBO, shaderID;
    unsigned int charVAO, charVBO;
    
public:
    TextRenderer(unsigned int width, unsigned int height);
    ~TextRenderer();
    
    void RenderHUD(int health, int score, int ammo, int reserved_mags, unsigned int screenW, unsigned int screenH);
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color, unsigned int screenW, unsigned int screenH);
    void RenderPauseText(unsigned int screenW, unsigned int screenH);
    void RenderStartText(unsigned int screenW, unsigned int screenH);
    void RenderEndText(int score, bool won, unsigned int screenW, unsigned int screenH);
    
private:
    void renderChar(char c, float& x, float y, float scale, glm::vec3 color, unsigned int screenW, unsigned int screenH);
    glm::vec3 getCharColor(char c);
};
