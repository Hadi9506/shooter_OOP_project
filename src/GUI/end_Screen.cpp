#include "end_Screen.h"

// Constructor for end button
// Initializes button with position, size, colors, and label
EndButton::EndButton(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 textCol, std::string lbl)
    : id(buttonId), position(pos), size(sz), color(col), hoverColor(hovCol), textColor(textCol), label(lbl), isHovered(false) {}

// Check if given mouse position is over this button bounds
bool EndButton::isMouseOver(glm::vec2 mousePos) const {
    return mousePos.x >= position.x - size.x / 2 &&
           mousePos.x <= position.x + size.x / 2 &&
           mousePos.y >= position.y - size.y / 2 &&
           mousePos.y <= position.y + size.y / 2;
}

// Constructor for end screen
// Initializes screen dimensions and creates shader, VAO, and buttons
EndScreen::EndScreen(float width, float height)
    : screenWidth(width), screenHeight(height), mousePos(0.0f), lastClickedButton(-1), mousePressed(false), 
      playerWon(false), finalScore(0) {
    initShaders();
    createQuadVAO();
    createButtons();
}

// Initialize 2D shader program for rendering rectangles and buttons
void EndScreen::initShaders() {
    // Vertex shader for 2D transformation
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 position;
        
        uniform mat4 projection;
        uniform mat4 model;
        
        void main() {
            gl_Position = projection * model * vec4(position, 0.0, 1.0);
        }
    )";
    
    // Fragment shader for color rendering
    const char* fragmentShaderSource = R"(
        #version 330 core
        uniform vec3 color;
        
        out vec4 FragColor;
        
        void main() {
            FragColor = vec4(color, 1.0);
        }
    )";
    
    // Compile vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    // Check for vertex shader compilation errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "End Screen - Vertex Shader Compilation Failed: " << infoLog << std::endl;
    }
    
    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    // Check for fragment shader compilation errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "End Screen - Fragment Shader Compilation Failed: " << infoLog << std::endl;
    }
    
    // Link vertex and fragment shaders into final program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    // Check for program linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "End Screen - Shader Program Linking Failed: " << infoLog << std::endl;
    }
    
    // Clean up shader objects after linking
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Create vertex array object with quad vertices for rendering rectangles
void EndScreen::createQuadVAO() {
    // Simple quad covering unit square (-1 to 1 on both axes)
    float quadVertices[] = {
        -1.0f,  1.0f,   // Top left
         1.0f,  1.0f,   // Top right
         1.0f, -1.0f,   // Bottom right
         1.0f, -1.0f,   // Bottom right (duplicate for second triangle)
        -1.0f, -1.0f,   // Bottom left
        -1.0f,  1.0f    // Top left (duplicate for second triangle)
    };
    
    // Create vertex array object and buffer
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    // Bind and fill buffer with vertex data
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    // Set vertex attribute pointer for position
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    // Unbind buffers
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Create all buttons for end screen menu
void EndScreen::createButtons() {
    buttons.clear();
    
    float centerX = screenWidth / 2;
    float centerY = screenHeight / 2;
    
    // Play Again button (green) positioned above center
    buttons.push_back(EndButton(
        0, glm::vec2(centerX, centerY - 60),
        glm::vec2(300, 80),
        glm::vec3(0.2f, 0.8f, 0.3f),      // Normal green color
        glm::vec3(0.3f, 1.0f, 0.4f),      // Bright green on hover
        glm::vec3(1.0f, 1.0f, 1.0f),      // White text
        "PLAY AGAIN"
    ));
    
    // Main menu button (red) positioned below center
    buttons.push_back(EndButton(
        1, glm::vec2(centerX, centerY + 60),
        glm::vec2(300, 80),
        glm::vec3(0.9f, 0.2f, 0.2f),      // Normal red color
        glm::vec3(1.0f, 0.3f, 0.3f),      // Bright red on hover
        glm::vec3(1.0f, 1.0f, 1.0f),      // White text
        "MAIN MENU"
    ));
}

// Update button hover states based on current mouse position
void EndScreen::updateMouse(double xpos, double ypos) {
    mousePos = glm::vec2(xpos, ypos);
    
    // Check hover state for each button
    for (auto& button : buttons) {
        button.isHovered = button.isMouseOver(mousePos);
    }
}

// Called when mouse button is pressed
void EndScreen::handleMousePress() {
    mousePressed = true;
}

// Called when mouse button is released
void EndScreen::handleMouseRelease() {
    mousePressed = false;
}

// Check if any button was clicked and return its ID
int EndScreen::handleButtonClick() {
    if (!mousePressed) return -1;
    
    // Check each button for click
    for (const auto& button : buttons) {
        if (button.isMouseOver(mousePos)) {
            mousePressed = false;
            return button.id;
        }
    }
    mousePressed = false;
    return -1;
}

// Store game result (won/lost) and final score for display
void EndScreen::setGameResult(bool won, int score) {
    playerWon = won;
    finalScore = score;
}

// Render a filled rectangle at specified position with given color
void EndScreen::renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color) {
    // Create orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
    
    // Create model matrix for rectangle transformation
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
    model = glm::scale(model, glm::vec3(size.x / 2, size.y / 2, 1.0f));
    
    // Activate shader and set uniforms
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.x, color.y, color.z);
    
    // Render the quad
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// Render a single button with normal or hover color
void EndScreen::renderButton(const EndButton& button) {
    glm::vec3 buttonColor = button.isHovered ? button.hoverColor : button.color;
    renderRectangle(button.position, button.size, buttonColor);
}

// Render text (placeholder for future text rendering implementation)
void EndScreen::renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    // Text rendering will be implemented via TextRenderer class in main
    std::cout << text << std::endl;
}

// Render title panel and background with appropriate colors
void EndScreen::renderTitle() {
    // Dark semi-transparent background overlay covering entire screen
    renderRectangle(glm::vec2(screenWidth / 2, screenHeight / 2), 
                   glm::vec2(screenWidth, screenHeight), 
                   glm::vec3(0.0f, 0.0f, 0.0f));
    
    // Main panel (green if player won, red if player lost)
    glm::vec3 panelColor = playerWon ? glm::vec3(0.1f, 0.3f, 0.1f) : glm::vec3(0.3f, 0.1f, 0.1f);
    renderRectangle(glm::vec2(screenWidth / 2, screenHeight / 2 - 50), 
                   glm::vec2(700, 500), 
                   panelColor);
}

// Render entire end screen with background, panel, and buttons
void EndScreen::renderEndScreen(int score, bool won) {
    // Clear screen with black color
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    // Render background and title panel
    renderTitle();
    
    // Render all buttons
    for (const auto& button : buttons) {
        renderButton(button);
    }
    
    // Re-enable depth testing for 3D rendering
    glEnable(GL_DEPTH_TEST);
}

// Destructor cleans up OpenGL resources
EndScreen::~EndScreen() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(shaderProgram);
}