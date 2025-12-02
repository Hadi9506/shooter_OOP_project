#ifndef END_SCREEN_H
#define END_SCREEN_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>

// End screen button structure
// Stores position, size, colors, and state for end game menu buttons
struct EndButton {
    glm::vec2 position;           // Center position on screen
    glm::vec2 size;               // Button width and height
    glm::vec3 color;              // Normal button color
    glm::vec3 hoverColor;         // Color when mouse hovers over button
    glm::vec3 textColor;          // Text label color
    std::string label;            // Button label text
    bool isHovered;               // Whether mouse is currently over button
    int id;                       // Button identifier
    
    // Constructor initializes button with all parameters
    EndButton(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 textCol, std::string lbl);
    
    // Check if given mouse position is within button bounds
    bool isMouseOver(glm::vec2 mousePos) const;
};

// End screen class manages the game over screen interface
// Displays final score and allows player to restart or return to menu
class EndScreen {
public:
    std::vector<EndButton> buttons;   // All buttons on the end screen
    glm::vec2 mousePos;               // Current mouse position
    float screenWidth, screenHeight;  // Screen dimensions
    unsigned int quadVAO, quadVBO;    // Vertex array and buffer for rectangles
    unsigned int shaderProgram;       // OpenGL shader program for rendering
    int lastClickedButton;            // Last button that was clicked
    bool mousePressed;                // Whether mouse button is currently pressed
    bool playerWon;                   // Whether player won the game
    int finalScore;                   // Final score value to display
    
    // Constructor initializes end screen with screen dimensions
    EndScreen(float width, float height);
    
    // Destructor cleans up OpenGL resources
    ~EndScreen();
    
    // Initialize 2D shader program for rendering
    void initShaders();
    
    // Create vertex array object with quad vertices
    void createQuadVAO();
    
    // Create all buttons for end screen (Play Again, Main Menu)
    void createButtons();
    
    // Update button hover states based on mouse position
    void updateMouse(double xpos, double ypos);
    
    // Check if button was clicked and return button ID
    int handleButtonClick();
    
    // Called when mouse button is pressed
    void handleMousePress();
    
    // Called when mouse button is released
    void handleMouseRelease();
    
    // Render entire end screen with buttons
    void renderEndScreen(int score, bool won);
    
    // Render a filled rectangle at specified position with color
    void renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color);
    
    // Render a single button with hover color
    void renderButton(const EndButton& button);
    
    // Render title panel and background
    void renderTitle();
    
    // Render text (placeholder for future text rendering)
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    
    // Store game result (won/lost) and final score
    void setGameResult(bool won, int score);
};

#endif // END_SCREEN_H