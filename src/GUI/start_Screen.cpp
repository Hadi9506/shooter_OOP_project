#include "start_Screen.h"

// Constructor for start button
// Initializes position, size, colors, and label
StartButton::StartButton(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 textCol, std::string lbl)
    : id(buttonId), position(pos), size(sz), color(col), hoverColor(hovCol), textColor(textCol), label(lbl), isHovered(false) {}

// Check if given mouse position is over this button
bool StartButton::isMouseOver(glm::vec2 mousePos) const {
    return mousePos.x >= position.x - size.x / 2 &&
           mousePos.x <= position.x + size.x / 2 &&
           mousePos.y >= position.y - size.y / 2 &&
           mousePos.y <= position.y + size.y / 2;
}

// Constructor: initialize start screen with dimensions
StartScreen::StartScreen(float width, float height)
    : screenWidth(width), screenHeight(height), mousePos(0.0f), lastClickedButton(-1), mousePressed(false) {
    initShaders();
    createQuadVAO();
    createButtons();
}

// Initialize 2D shader program for rendering rectangles
void StartScreen::initShaders() {
    const char* vertexShaderSource = R"(
        #version 330 core
        layout(location = 0) in vec2 position;
        
        uniform mat4 projection;
        uniform mat4 model;
        
        void main() {
            gl_Position = projection * model * vec4(position, 0.0, 1.0);
        }
    )";
    
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
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "Vertex Shader Compilation Failed: " << infoLog << std::endl;
    }
    
    // Compile fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "Fragment Shader Compilation Failed: " << infoLog << std::endl;
    }
    
    // Link shader program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "Shader Program Linking Failed: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

// Create vertex array object with quad vertices
void StartScreen::createQuadVAO() {
    // Simple quad covering unit square
    float quadVertices[] = {
        -1.0f,  1.0f,
         1.0f,  1.0f,
         1.0f, -1.0f,
         1.0f, -1.0f,
        -1.0f, -1.0f,
        -1.0f,  1.0f
    };
    
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

// Create all start menu buttons
void StartScreen::createButtons() {
    buttons.clear();
    
    float centerX = screenWidth / 2;
    float centerY = screenHeight / 2;
    
    // Play button (green) - top button
    buttons.push_back(StartButton(
        0, glm::vec2(centerX, centerY - 60),
        glm::vec2(300, 80),
        glm::vec3(0.2f, 0.8f, 0.3f),      // Green
        glm::vec3(0.3f, 1.0f, 0.4f),      // Bright green on hover
        glm::vec3(1.0f, 1.0f, 1.0f),      // White text
        "START GAME"
    ));
    
    // Exit button (red) - bottom button
    buttons.push_back(StartButton(
        1, glm::vec2(centerX, centerY + 60),
        glm::vec2(300, 80),
        glm::vec3(0.9f, 0.2f, 0.2f),      // Red
        glm::vec3(1.0f, 0.3f, 0.3f),      // Bright red on hover
        glm::vec3(1.0f, 1.0f, 1.0f),      // White text
        "EXIT GAME"
    ));
}

// Update mouse position and button hover states
void StartScreen::updateMouse(double xpos, double ypos) {
    mousePos = glm::vec2(xpos, ypos);
    
    // Update hover state for all buttons
    for (auto& button : buttons) {
        button.isHovered = button.isMouseOver(mousePos);
    }
}

// Handle mouse press
void StartScreen::handleMousePress() {
    mousePressed = true;
}

// Handle mouse release
void StartScreen::handleMouseRelease() {
    mousePressed = false;
}

// Check for button click and return ID
int StartScreen::handleButtonClick() {
    if (!mousePressed) return -1;
    
    // Check which button was clicked
    for (const auto& button : buttons) {
        if (button.isMouseOver(mousePos)) {
            mousePressed = false;
            return button.id;
        }
    }
    mousePressed = false;
    return -1;
}

// Render a filled rectangle on screen
void StartScreen::renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color) {
    // Create orthographic projection for 2D rendering
    glm::mat4 projection = glm::ortho(0.0f, screenWidth, screenHeight, 0.0f, -1.0f, 1.0f);
    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(position.x, position.y, 0.0f));
    model = glm::scale(model, glm::vec3(size.x / 2, size.y / 2, 1.0f));
    
    glUseProgram(shaderProgram);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, &model[0][0]);
    glUniform3f(glGetUniformLocation(shaderProgram, "color"), color.x, color.y, color.z);
    
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
}

// Render a button with hover color
void StartScreen::renderButton(const StartButton& button) {
    glm::vec3 buttonColor = button.isHovered ? button.hoverColor : button.color;
    renderRectangle(button.position, button.size, buttonColor);
}

// Render title and background
void StartScreen::renderTitle() {
    // Background panel for title
    renderRectangle(glm::vec2(screenWidth / 2, 100), 
                   glm::vec2(screenWidth * 0.8f, 150), 
                   glm::vec3(0.1f, 0.15f, 0.3f));
    
    // Background gradient effect
    renderRectangle(glm::vec2(screenWidth / 2, screenHeight / 2), 
                   glm::vec2(screenWidth, screenHeight), 
                   glm::vec3(0.05f, 0.05f, 0.1f));
}

// Render text (placeholder)
void StartScreen::renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
    // Placeholder for text rendering
}

// Render entire start screen
void StartScreen::renderStartScreen() {
    glClearColor(0.05f, 0.05f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    // Render background and title
    renderTitle();
    
    // Render all buttons
    for (const auto& button : buttons) {
        renderButton(button);
    }
    
    glEnable(GL_DEPTH_TEST);
}

// Destructor: clean up OpenGL resources
StartScreen::~StartScreen() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(shaderProgram);
}