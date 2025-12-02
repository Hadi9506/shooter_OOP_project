#ifndef START_SCREEN_H
#define START_SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>

// Button structure for start menu
// Stores position, size, colors, and state
struct StartButton {
    glm::vec2 position;        // Center position on screen
    glm::vec2 size;            // Width and height
    glm::vec3 color;           // Normal button color
    glm::vec3 hoverColor;      // Color when mouse is over
    glm::vec3 textColor;       // Text color
    std::string label;         // Button label text
    bool isHovered;            // Whether mouse is over button
    int id;                    // Button identifier
    
    // Constructor
    StartButton(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 textCol, std::string lbl);
    
    // Check if mouse position is over button
    bool isMouseOver(glm::vec2 mousePos) const;
};

// Start screen class manages the main menu interface
// Handles button rendering and mouse interaction
class StartScreen {
public:
    std::vector<StartButton> buttons;  // All buttons on screen
    glm::vec2 mousePos;                // Current mouse position
    float screenWidth, screenHeight;   // Screen dimensions
    unsigned int quadVAO, quadVBO;     // Vertex array for rectangles
    unsigned int shaderProgram;        // Shader program for rendering
    int lastClickedButton;             // Last clicked button ID
    bool mousePressed;                 // Whether mouse is currently pressed
    
    // Constructor
    StartScreen(float width, float height);
    ~StartScreen();
    
    // Initialize shader program
    void initShaders();
    
    // Create vertex array for rendering rectangles
    void createQuadVAO();
    
    // Create all menu buttons
    void createButtons();
    
    // Update mouse position and hover states
    void updateMouse(double xpos, double ypos);
    
    // Check for button click and return clicked button ID
    int handleButtonClick();
    
    // Called when mouse button is pressed
    void handleMousePress();
    
    // Called when mouse button is released
    void handleMouseRelease();
    
    // Render the entire start screen
    void renderStartScreen();
    
    // Render a filled rectangle at specified position
    void renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color);
    
    // Render a single button
    void renderButton(const StartButton& button);
    
    // Render title and background
    void renderTitle();
    
    // Render text (placeholder)
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
};

#endif // START_SCREEN_H