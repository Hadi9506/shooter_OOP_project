#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "start_Screen.h"

// ===================== SCREEN ENUM =====================
enum class GameScreen {
    START_MENU,
    GAMEPLAY,
    PAUSE_MENU,
    END_SCREEN,
    SETTINGS,
    NONE
};

// ===================== BUTTON STRUCT =====================
struct Button {
    glm::vec2 position;
    glm::vec2 size;
    glm::vec3 color;
    glm::vec3 hoverColor;
    glm::vec3 pressedColor;
    std::string label;
    bool isHovered;
    bool isPressed;
    int id;
    
    Button(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 pressCol, std::string lbl);
    bool isMouseOver(glm::vec2 mousePos);
    bool checkClick(glm::vec2 mousePos);
};

// ===================== GUI MANAGER CLASS =====================
class GUIManager {
public:
    GameScreen currentScreen;
    GameScreen previousScreen;
    std::vector<Button> buttons;
    glm::vec2 mousePos;
    float screenWidth, screenHeight;
    unsigned int quadVAO, quadVBO;
    unsigned int textVAO, textVBO;
    unsigned int shaderProgram;
    int lastClickedButton;
    bool mouseClicked;
    
    GUIManager(float width, float height);
    ~GUIManager();
    
    // Shader & VAO Setup
    void initShaders();
    void createQuadVAO();
    void createTextVAO();
    
    // Button Creation
    void createStartMenuButtons();
    void createPauseMenuButtons();
    void createEndScreenButtons(bool playerWon);
    void createSettingsMenuButtons();
    
    // Input Handling
    void updateMouse(double xpos, double ypos);
    int handleButtonClick();
    void handleMousePress();
    void handleMouseRelease();
    
    // Rendering - Main Screens
    void renderStartMenu();
    void renderPauseMenu();
    void renderEndScreen(int playerHealth, int score, bool playerWon);
    void renderSettingsMenu();
    void renderGameplayHUD(int playerHealth, int score, int ammo);
    
    // Rendering - Utilities
    void renderButton(const Button& button);
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    void renderBackground(glm::vec3 color);
    void renderPanel(glm::vec2 position, glm::vec2 size, glm::vec3 color, float alpha = 0.8f);
    void renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color);
    
    // Screen Management
    int processScreen();
    void setScreen(GameScreen screen);
    GameScreen getScreen() const;
};

// ===================== GLOBAL FUNCTION DECLARATIONS =====================
void initializeGUI(float screenWidth, float screenHeight);
void renderStartMenuScreen();
int handleStartMenuClick();
void updateStartMenuMouse(double xpos, double ypos);
void cleanupGUI();

// ===================== CALLBACK FUNCTION DECLARATIONS =====================
void guiMouseCallback(GLFWwindow* window, double xpos, double ypos);
void guiClickCallback(GLFWwindow* window, int button, int action, int mods);

#endif // MAIN_GUI_H