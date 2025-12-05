#include "end_Screen.h"

EndButton::EndButton(int buttonId, glm::vec2 pos, glm::vec2 sz, glm::vec3 col, glm::vec3 hovCol, glm::vec3 textCol, std::string lbl)
    : id(buttonId), position(pos), size(sz), color(col), hoverColor(hovCol), textColor(textCol), label(lbl), isHovered(false) {}

bool EndButton::isMouseOver(glm::vec2 mousePos) const {
    return mousePos.x >= position.x - size.x / 2 &&
           mousePos.x <= position.x + size.x / 2 &&
           mousePos.y >= position.y - size.y / 2 &&
           mousePos.y <= position.y + size.y / 2;
}

EndScreen::EndScreen(float width, float height)
    : screenWidth(width), screenHeight(height), mousePos(0.0f), lastClickedButton(-1), mousePressed(false), 
      playerWon(false), finalScore(0) {
    initShaders();
    createQuadVAO();
    createButtons();
}

void EndScreen::initShaders() {
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
    
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cerr << "End Screen - Vertex Shader Compilation Failed: " << infoLog << std::endl;
    }
    
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cerr << "End Screen - Fragment Shader Compilation Failed: " << infoLog << std::endl;
    }
    
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cerr << "End Screen - Shader Program Linking Failed: " << infoLog << std::endl;
    }
    
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void EndScreen::createQuadVAO() {
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

void EndScreen::createButtons() {
    buttons.clear();
    
    float centerX = screenWidth / 2;
    float centerY = screenHeight / 2;
    
    // Main menu button (white) at top
    buttons.push_back(EndButton(
        0, glm::vec2(centerX, 100),
        glm::vec2(300, 70),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.85f, 0.85f, 0.85f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        "MAIN MENU"
    ));
    
    // Restart button (green) at center-left
    buttons.push_back(EndButton(
        1, glm::vec2(centerX - 200, centerY + 150),
        glm::vec2(300, 80),
        glm::vec3(0.2f, 0.8f, 0.3f),
        glm::vec3(0.3f, 1.0f, 0.4f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        "RESTART"
    ));
    
    // Exit button (red) at center-right
    buttons.push_back(EndButton(
        2, glm::vec2(centerX + 200, centerY + 150),
        glm::vec2(300, 80),
        glm::vec3(0.9f, 0.2f, 0.2f),
        glm::vec3(1.0f, 0.3f, 0.3f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        "EXIT"
    ));
}

void EndScreen::updateMouse(double xpos, double ypos) {
    mousePos = glm::vec2(xpos, ypos);
    
    for (auto& button : buttons) {
        button.isHovered = button.isMouseOver(mousePos);
    }
}

void EndScreen::handleMousePress() {
    mousePressed = true;
}

void EndScreen::handleMouseRelease() {
    mousePressed = false;
}

int EndScreen::handleButtonClick() {
    if (!mousePressed) return -1;
    
    for (const auto& button : buttons) {
        if (button.isMouseOver(mousePos)) {
            mousePressed = false;
            return button.id;
        }
    }
    mousePressed = false;
    return -1;
}

void EndScreen::setGameResult(bool won, int score) {
    playerWon = won;
    finalScore = score;
}

void EndScreen::renderRectangle(glm::vec2 position, glm::vec2 size, glm::vec3 color) {
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

void EndScreen::renderButton(const EndButton& button) {
    glm::vec3 buttonColor = button.isHovered ? button.hoverColor : button.color;
    renderRectangle(button.position, button.size, buttonColor);
}

void EndScreen::renderText(const std::string& text, float x, float y, float scale, glm::vec3 color) {
}

void EndScreen::renderTitle() {
    renderRectangle(glm::vec2(screenWidth / 2, screenHeight / 2), 
                   glm::vec2(screenWidth, screenHeight), 
                   glm::vec3(0.0f, 0.0f, 0.0f));
    
    glm::vec3 panelColor = playerWon ? glm::vec3(0.1f, 0.3f, 0.1f) : glm::vec3(0.3f, 0.1f, 0.1f);
    renderRectangle(glm::vec2(screenWidth / 2, screenHeight / 2), 
                   glm::vec2(800, 400), 
                   panelColor);
}

void EndScreen::renderEndScreen(int score, bool won) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    
    renderTitle();
    
    for (const auto& button : buttons) {
        renderButton(button);
    }
    
    glEnable(GL_DEPTH_TEST);
}

EndScreen::~EndScreen() {
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteProgram(shaderProgram);
}