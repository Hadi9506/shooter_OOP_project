#pragma once
#include <glm/glm.hpp>
#include <string>

class TextRenderer {
    unsigned int VAO,VBO,shaderID;

    public:
        TextRenderer(unsigned int width, unsigned int height);
        ~TextRenderer();
        void RenderHUD(int health, int score, int ammo, unsigned int screenW, unsigned int screenH);
};
