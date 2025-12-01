#ifndef END_SCREEN_H
#define END_SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>

struct EndButton {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;
    glm::vec3 hoverColor;
    glm::vec3 textColor;
    std::string label;
    bool isHovered;
    int id;
    
    EndButton(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 textCol, std::string lbl);
    bool isMouseOver(glm::vec2 mousePos) const;
};

class EndScreen {
public:
    std::vector<EndButton> buttons;
    glm::vec2 mousePos;
    float screenWidth, screenHeight;
    unsigned int quadVAO, quadVBO;
    unsigned int shaderProgram;
    int lastClickedButton;
    bool mousePressed;
    bool playerWon;
    int finalScore;
    
    EndScreen(float width, float height);
    ~EndScreen();
    
    void initShaders();
    void createQuadVAO();
    void createButtons();
    void updateMouse(double xpos, double ypos);
    int handleButtonClick();
    void handleMousePress();
    void handleMouseRelease();
    void renderEndScreen(int score, bool won);
    void renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color);
    void renderButton(const EndButton& button);
    void renderTitle();
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    void setGameResult(bool won, int score);
};

#endif // END_SCREEN_H