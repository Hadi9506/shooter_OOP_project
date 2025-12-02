#ifndef PAUSE_SCREEN_H
#define PAUSE_SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>

// Pause screen button structure
// Stores button position, size, colors, and state
struct PauseButton {
    glm::vec2 position;           // Center position on screen
    glm::vec2 size;               // Button width and height
    glm::vec3 color;              // Normal button color
    glm::vec3 hoverColor;         // Color when mouse hovers
    glm::vec3 textColor;          // Text label color
    std::string label;            // Button label text
    bool isHovered;               // Whether mouse is over button
    int id;                       // Button identifier
    
    // Constructor initializes button with all parameters
    PauseButton(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 textCol, std::string lbl);
    
    // Check if given mouse position is within button bounds
    bool isMouseOver(glm::vec2 mousePos) const;
};

// Pause screen class manages the pause menu interface
// Displays options to resume, restart, or return to main menu
class PauseScreen {
public:
    std::vector<PauseButton> buttons;  // All buttons on pause menu
    glm::vec2 mousePos;                // Current mouse position
    float screenWidth, screenHeight;   // Screen dimensions
    unsigned int quadVAO, quadVBO;     // Vertex array and buffer for rectangles
    unsigned int shaderProgram;        // OpenGL shader program
    int lastClickedButton;             // Last button clicked
    bool mousePressed;                 // Whether mouse button is pressed
    
    // Constructor initializes pause screen with screen dimensions
    PauseScreen(float width, float height);
    
    // Destructor cleans up OpenGL resources
    ~PauseScreen();
    
    // Initialize 2D shader program for rendering
    void initShaders();
    
    // Create vertex array object with quad vertices
    void createQuadVAO();
    
    // Create all buttons for pause menu (Resume, Restart, Main Menu)
    void createButtons();
    
    // Update button hover states based on mouse position
    void updateMouse(double xpos, double ypos);
    
    // Check if button was clicked and return button ID
    int handleButtonClick();
    
    // Called when mouse button is pressed
    void handleMousePress();
    
    // Called when mouse button is released
    void handleMouseRelease();
    
    // Render entire pause screen with buttons and background
    void renderPauseScreen();
    
    // Render a filled rectangle at specified position
    void renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color);
    
    // Render a single button with hover color
    void renderButton(const PauseButton& button);
    
    // Render title panel and background overlay
    void renderTitle();
    
    // Render text (placeholder for future text rendering)
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
};

#endif // PAUSE_SCREEN_H