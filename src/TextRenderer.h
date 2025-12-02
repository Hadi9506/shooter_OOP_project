#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>

// TextRenderer class handles all text rendering in the game
// Renders HUD, menu text, pause text, start text, and end game text
class TextRenderer {
private:
    unsigned int VAO, VBO;          // Vertex array and buffer for rectangles
    unsigned int shaderID;          // Shader program for text rendering
    unsigned int charVAO, charVBO;  // Character-specific vertex arrays
    
public:
    // Constructor initializes shader and vertex arrays with screen dimensions
    TextRenderer(unsigned int width, unsigned int height);
    
    // Destructor cleans up OpenGL resources
    ~TextRenderer();
    
    // Render in-game HUD with health, score, and ammunition information
    void RenderHUD(int health, int score, int ammo, int reserved_mags, unsigned int screenW, unsigned int screenH);
    
    // Render arbitrary text string on screen
    void RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color, unsigned int screenW, unsigned int screenH);
    
    // Render pause screen text overlay
    void RenderPauseText(unsigned int screenW, unsigned int screenH);
    
    // Render start menu title text
    void RenderStartText(unsigned int screenW, unsigned int screenH);
    
    // Render end game result text and final score
    void RenderEndText(int score, bool won, unsigned int screenW, unsigned int screenH);
    
private:
    // Render a single character and advance x position for next character
    void renderChar(char c, float& x, float y, float scale, glm::vec3 color, unsigned int screenW, unsigned int screenH);
    
    // Get color for a specific character (placeholder for future use)
    glm::vec3 getCharColor(char c);
};
