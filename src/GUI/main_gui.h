#ifndef MAIN_GUI_H
#define MAIN_GUI_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "start_Screen.h"
#include "pause_Screen.h"
#include "end_Screen.h"

// Game screen enumeration for state management
// Tracks which screen the game is currently displaying
enum class GameScreen {
    START_MENU,     // Main menu screen
    GAMEPLAY,       // Active gameplay
    PAUSE_MENU,     // Pause menu overlay
    END_SCREEN,     // Game over screen
    SETTINGS,       // Settings menu (for future use)
    NONE            // No screen active
};

// Button structure for GUI menu buttons
// Stores position, size, colors, and interaction state
struct Button {
    glm::vec2 position;         // Center position on screen
    glm::vec2 size;             // Button width and height
    glm::vec3 color;            // Normal button color
    glm::vec3 hoverColor;       // Color when mouse hovers
    glm::vec3 pressedColor;     // Color when pressed
    std::string label;          // Button label text
    bool isHovered;             // Whether mouse is over button
    bool isPressed;             // Whether button is currently pressed
    int id;                     // Button identifier
    
    // Constructor initializes button with all parameters
    Button(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 pressCol, std::string lbl);
    
    // Check if mouse position is within button bounds
    bool isMouseOver(glm::vec2 mousePos);
    
    // Check if button was clicked
    bool checkClick(glm::vec2 mousePos);
};

// GUI Manager class handles all menu screens and user interface
// Manages screen transitions, button rendering, and input handling
class GUIManager {
public:
    GameScreen currentScreen;           // Currently active screen
    GameScreen previousScreen;          // Previous screen for back navigation
    std::vector<Button> buttons;        // All buttons on current screen
    glm::vec2 mousePos;                 // Current mouse position
    float screenWidth, screenHeight;    // Screen dimensions
    unsigned int quadVAO, quadVBO;      // Vertex array for rectangles
    unsigned int textVAO, textVBO;      // Vertex array for text
    unsigned int shaderProgram;         // OpenGL shader program
    int lastClickedButton;              // Last button that was clicked
    bool mouseClicked;                  // Whether mouse button is clicked
    
    // Constructor initializes GUI manager with screen dimensions
    GUIManager(float width, float height);
    
    // Destructor cleans up OpenGL resources
    ~GUIManager();
    
    // Initialize 2D shader program for rendering
    void initShaders();
    
    // Create vertex array object for rectangles
    void createQuadVAO();
    
    // Create vertex array object for text rendering
    void createTextVAO();
    
    // Create buttons for start menu
    void createStartMenuButtons();
    
    // Create buttons for pause menu
    void createPauseMenuButtons();
    
    // Create buttons for end screen
    void createEndScreenButtons(bool playerWon);
    
    // Create buttons for settings menu
    void createSettingsMenuButtons();
    
    // Update button hover states based on mouse position
    void updateMouse(double xpos, double ypos);
    
    // Check if button was clicked and return button ID
    int handleButtonClick();
    
    // Called when mouse button is pressed
    void handleMousePress();
    
    // Called when mouse button is released
    void handleMouseRelease();
    
    // Render start menu screen
    void renderStartMenu();
    
    // Render pause menu screen
    void renderPauseMenu();
    
    // Render end game screen with final score and result
    void renderEndScreen(int playerHealth, int score, bool playerWon);
    
    // Render settings menu
    void renderSettingsMenu();
    
    // Render in-game HUD with player stats
    void renderGameplayHUD(int playerHealth, int score, int ammo);
    
    // Render a single button
    void renderButton(const Button& button);
    
    // Render text on screen
    void renderText(const std::string& text, float x, float y, float scale, glm::vec3 color);
    
    // Render background color
    void renderBackground(glm::vec3 color);
    
    // Render semi-transparent panel
    void renderPanel(glm::vec2 position, glm::vec2 size, glm::vec3 color, float alpha = 0.8f);
    
    // Render rectangle primitive
    void renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color);
    
    // Process current screen input and return result
    int processScreen();
    
    // Set active screen
    void setScreen(GameScreen screen);
    
    // Get currently active screen
    GameScreen getScreen() const;
};

// Global GUI functions for screen management

// Initialize GUI system with screen dimensions
void initializeGUI(float screenWidth, float screenHeight);

// Render start menu screen
void renderStartMenuScreen();

// Render pause menu screen
void renderPauseMenuScreen();

// Render end menu screen
void renderEndMenuScreen();

// Check for start menu button click and return ID
int handleStartMenuClick();

// Check for pause menu button click and return ID
int handlePauseMenuClick();

// Check for end menu button click and return ID
int handleEndMenuClick();

// Update start menu mouse position
void updateStartMenuMouse(double xpos, double ypos);

// Update pause menu mouse position
void updatePauseMenuMouse(double xpos, double ypos);

// Update end menu mouse position
void updateEndMenuMouse(double xpos, double ypos);

// Called when mouse button pressed on start menu
void handleStartMenuMousePress();

// Called when mouse button released on start menu
void handleStartMenuMouseRelease();

// Called when mouse button pressed on pause menu
void handlePauseMenuMousePress();

// Called when mouse button released on pause menu
void handlePauseMenuMouseRelease();

// Called when mouse button pressed on end menu
void handleEndMenuMousePress();

// Called when mouse button released on end menu
void handleEndMenuMouseRelease();

// Set end screen game result (won/lost) and final score
void setEndScreenResult(bool won, int score);

// Clean up and deallocate GUI resources
void cleanupGUI();

// GLFW callback function for mouse movement
void guiMouseCallback(GLFWwindow* window, double xpos, double ypos);

// GLFW callback function for mouse button clicks
void guiClickCallback(GLFWwindow* window, int button, int action, int mods);

#endif // MAIN_GUI_H