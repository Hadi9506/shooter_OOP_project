#include "TextRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>

// Declare the external variables
extern int currentAmmo;
extern int reserveMags;
extern int partialMagAmmo;
extern int score;

// ✅ Simple bitmap font for rendering
bool charBitmap[128][8] = {
    // Space (32)
    {0,0,0,0,0,0,0,0},
    // '!' (33)
    {0,0,1,1,0,1,0,0},
    // Digits 0-9 (48-57)
    {1,1,1,1,1,1,1,1}, // 0
    {0,1,0,1,1,0,1,0}, // 1
    {1,0,1,1,0,1,1,1}, // 2
    {1,0,1,1,1,1,1,1}, // 3
    {1,1,1,0,0,1,0,1}, // 4
    {1,1,0,1,1,0,1,1}, // 5
    {1,1,0,1,1,1,1,1}, // 6
    {1,0,1,0,1,0,0,1}, // 7
    {1,1,1,1,1,1,1,1}, // 8
    {1,1,1,1,0,1,1,1}, // 9
    // ':' (58)
    {0,1,0,0,0,1,0,0},
    // '/' (47)
    {0,0,1,0,1,1,0,0},
    // '-' (45)
    {0,0,0,1,1,0,0,0},
};

TextRenderer::TextRenderer(unsigned int width, unsigned int height) {
    // Simple 2D colored bar shader
    const char* vs = R"(
        #version 330 core
        layout(location = 0) in vec2 pos;
        uniform mat4 proj;
        void main() { gl_Position = proj * vec4(pos, 0.0, 1.0); }
    )";

    const char* fs = R"(
        #version 330 core
        uniform vec3 color;
        out vec4 fragColor;
        void main() { fragColor = vec4(color, 1.0); }
    )";

    unsigned int v = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(v, 1, &vs, nullptr);
    glCompileShader(v);

    int success;
    char infoLog[512];
    glGetShaderiv(v, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(v, 512, nullptr, infoLog);
        std::cerr << "TextRenderer Vertex Shader Error: " << infoLog << std::endl;
    }

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, nullptr);
    glCompileShader(f);

    glGetShaderiv(f, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(f, 512, nullptr, infoLog);
        std::cerr << "TextRenderer Fragment Shader Error: " << infoLog << std::endl;
    }

    shaderID = glCreateProgram();
    glAttachShader(shaderID, v);
    glAttachShader(shaderID, f);
    glLinkProgram(shaderID);

    glGetProgramiv(shaderID, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderID, 512, nullptr, infoLog);
        std::cerr << "TextRenderer Shader Program Error: " << infoLog << std::endl;
    }

    glDeleteShader(v);
    glDeleteShader(f);

    // VAO / VBO for rectangles
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, nullptr, GL_DYNAMIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

TextRenderer::~TextRenderer() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderID);
}

// ✅ Helper: Draw Rectangle
void drawRect(float x, float y, float w, float h, glm::vec3 color, unsigned int shaderID, unsigned int VAO, unsigned int VBO, glm::mat4 proj) {
    glUseProgram(shaderID);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, &proj[0][0]);
    glUniform3fv(glGetUniformLocation(shaderID, "color"), 1, &color[0]);

    float verts[] = {
        x,     y,
        x + w, y,
        x,     y + h,
        x + w, y + h
    };

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

// ✅ Draw a single character using bitmap
void drawCharacter(char c, float x, float y, float scale, glm::vec3 color, unsigned int shaderID, unsigned int VAO, unsigned int VBO, glm::mat4 proj) {
    float cellW = 8.0f * scale;
    float cellH = 8.0f * scale;
    float dotSize = 2.0f * scale;

    // Get bitmap for character
    int charIdx = (int)c;
    if (charIdx < 0 || charIdx > 127) charIdx = 32; // Default to space
    
    bool* bits = charBitmap[charIdx];
    
    // Draw each pixel of the character
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (bits[row * 8 + col]) {
                float px = x + col * dotSize;
                float py = y + row * dotSize;
                drawRect(px, py, dotSize, dotSize, color, shaderID, VAO, VBO, proj);
            }
        }
    }
}

// ✅ RENDER CHARACTER
void TextRenderer::renderChar(char c, float& x, float y, float scale, glm::vec3 color, unsigned int screenW, unsigned int screenH) {
    glm::mat4 proj = glm::ortho(0.0f, (float)screenW, (float)screenH, 0.0f);
    drawCharacter(c, x, y, scale, color, shaderID, VAO, VBO, proj);
    x += 10.0f * scale;
}

// ✅ RENDER TEXT
void TextRenderer::RenderText(const std::string& text, float x, float y, float scale, glm::vec3 color, unsigned int screenW, unsigned int screenH) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(shaderID);

    float currentX = x;
    for (char c : text) {
        renderChar(c, currentX, y, scale, color, screenW, screenH);
    }

    glEnable(GL_DEPTH_TEST);
}

// ✅ RENDER HUD
void TextRenderer::RenderHUD(int health, int score, int ammo, int magz, unsigned int w, unsigned int h) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(shaderID);

    glm::mat4 proj = glm::ortho(0.0f, (float)w, (float)h, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, &proj[0][0]);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    float margin = 20.0f;
    float barY = h - 60.0f;
    float barH = 25.0f;

    // === 1. HEALTH BAR (GREEN → RED) ===
    float healthBgVerts[] = {
        margin - 3,     barY + 3,
        margin + 203,   barY + 3,
        margin - 3,     barY - barH - 3,
        margin + 203,   barY - barH - 3
    };
    glUniform3f(glGetUniformLocation(shaderID, "color"), 0.0f, 0.0f, 0.0f);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(healthBgVerts), healthBgVerts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    float healthPct = health / 100.0f;
    glm::vec3 healthColor = glm::mix(glm::vec3(1,0,0), glm::vec3(0,1,0), healthPct);
    float healthW = 200.0f * healthPct;
    float healthVerts[] = {
        margin,     barY,
        margin + healthW, barY,
        margin,     barY - barH,
        margin + healthW, barY - barH
    };
    glUniform3fv(glGetUniformLocation(shaderID, "color"), 1, &healthColor[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(healthVerts), healthVerts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // === 2. AMMO BARS ===
    float ammoStartX = margin + 230;
    float ammoBarW = 120.0f;
    float ammoBarH = 18.0f;
    float ammoSpacing = 22.0f;

    // Current Mag (YELLOW)
    float bar1Y = barY;
    float currentAmmoPct = (currentAmmo > 0) ? (currentAmmo / 30.0f) : 0.0f;

    float bar1BgVerts[] = {
        ammoStartX - 2,     bar1Y + 2,
        ammoStartX + ammoBarW + 2, bar1Y + 2,
        ammoStartX - 2,     bar1Y - ammoBarH - 2,
        ammoStartX + ammoBarW + 2, bar1Y - ammoBarH - 2
    };
    glUniform3f(glGetUniformLocation(shaderID, "color"), 0.0f, 0.0f, 0.0f);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bar1BgVerts), bar1BgVerts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    float bar1Verts[] = {
        ammoStartX, bar1Y,
        ammoStartX + ammoBarW * currentAmmoPct, bar1Y,
        ammoStartX, bar1Y - ammoBarH,
        ammoStartX + ammoBarW * currentAmmoPct, bar1Y - ammoBarH
    };
    glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 0.85f, 0.0f);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bar1Verts), bar1Verts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Reserve Mags (WHITE blocks)
    float bar2Y = bar1Y - ammoSpacing;
    float magBlockW = 15.0f;
    float magBlockSpacing = 3.0f;

    glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 1.0f, 1.0f);
    for (int i = 0; i < reserveMags && i < 7; i++) {
        float xPos = ammoStartX + i * (magBlockW + magBlockSpacing);
        float bar2Verts[] = {
            xPos, bar2Y,
            xPos + magBlockW, bar2Y,
            xPos, bar2Y - ammoBarH,
            xPos + magBlockW, bar2Y - ammoBarH
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bar2Verts), bar2Verts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // Partial Mag (ORANGE)
    float bar3Y = bar2Y - ammoSpacing;
    float partialAmmoPct = (partialMagAmmo > 0) ? (partialMagAmmo / 30.0f) : 0.0f;

    float bar3BgVerts[] = {
        ammoStartX - 2,     bar3Y + 2,
        ammoStartX + ammoBarW + 2, bar3Y + 2,
        ammoStartX - 2,     bar3Y - ammoBarH - 2,
        ammoStartX + ammoBarW + 2, bar3Y - ammoBarH - 2
    };
    glUniform3f(glGetUniformLocation(shaderID, "color"), 0.0f, 0.0f, 0.0f);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bar3BgVerts), bar3BgVerts);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    if (partialMagAmmo > 0) {
        float bar3Verts[] = {
            ammoStartX, bar3Y,
            ammoStartX + ammoBarW * partialAmmoPct, bar3Y,
            ammoStartX, bar3Y - ammoBarH,
            ammoStartX + ammoBarW * partialAmmoPct, bar3Y - ammoBarH
        };
        glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 0.5f, 0.0f);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bar3Verts), bar3Verts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }

    // === 3. TEXT LABELS ===
    // "HEALTH" label
    std::string healthLabel = "HEALTH";
    float labelX = margin;
    float labelY = barY + 35.0f;
    glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 1.0f, 1.0f);
    for (char c : healthLabel) {
        drawCharacter(c, labelX, labelY, 0.5f, glm::vec3(1,1,1), shaderID, VAO, VBO, glm::ortho(0.0f, (float)w, (float)h, 0.0f));
        labelX += 6.0f;
    }
    
    // "AMMO" label
    std::string ammoLabel = "AMMO";
    labelX = ammoStartX;
    labelY = barY + 35.0f;
    for (char c : ammoLabel) {
        drawCharacter(c, labelX, labelY, 0.5f, glm::vec3(1,1,1), shaderID, VAO, VBO, glm::ortho(0.0f, (float)w, (float)h, 0.0f));
        labelX += 6.0f;
    }

    // Score display (top right)
    std::stringstream ss;
    ss << "SCORE:" << std::setw(6) << std::setfill('0') << score;
    std::string scoreStr = ss.str();
    
    float scoreX = w - 150.0f;
    float scoreY = 20.0f;
    glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 1.0f, 0.0f);
    for (char c : scoreStr) {
        drawCharacter(c, scoreX, scoreY, 0.6f, glm::vec3(1,1,0), shaderID, VAO, VBO, glm::ortho(0.0f, (float)w, (float)h, 0.0f));
        scoreX += 7.0f;
    }

    glEnable(GL_DEPTH_TEST);
}

// ✅ RENDER PAUSE TEXT
void TextRenderer::RenderPauseText(unsigned int screenW, unsigned int screenH) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(shaderID);

    glm::mat4 proj = glm::ortho(0.0f, (float)screenW, (float)screenH, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, &proj[0][0]);

    // "PAUSED" title
    std::string pausedText = "PAUSED";
    float textX = screenW / 2.0f - pausedText.length() * 5.0f;
    float textY = 60.0f;

    glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 1.0f, 1.0f);
    for (char c : pausedText) {
        drawCharacter(c, textX, textY, 1.5f, glm::vec3(1,1,1), shaderID, VAO, VBO, proj);
        textX += 15.0f;
    }

    glEnable(GL_DEPTH_TEST);
}

// ✅ RENDER START TEXT
void TextRenderer::RenderStartText(unsigned int screenW, unsigned int screenH) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(shaderID);

    glm::mat4 proj = glm::ortho(0.0f, (float)screenW, (float)screenH, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, &proj[0][0]);

    // Title
    std::string titleText = "SHOOTER GAME";
    float textX = screenW / 2.0f - titleText.length() * 7.0f;
    float textY = 80.0f;

    glUniform3f(glGetUniformLocation(shaderID, "color"), 0.0f, 1.0f, 1.0f); // Cyan
    for (char c : titleText) {
        drawCharacter(c, textX, textY, 1.2f, glm::vec3(0,1,1), shaderID, VAO, VBO, proj);
        textX += 12.0f;
    }

    glEnable(GL_DEPTH_TEST);
}

// ✅ RENDER END TEXT
void TextRenderer::RenderEndText(int finalScore, bool won, unsigned int screenW, unsigned int screenH) {
    glDisable(GL_DEPTH_TEST);
    glUseProgram(shaderID);

    glm::mat4 proj = glm::ortho(0.0f, (float)screenW, (float)screenH, 0.0f);
    glUniformMatrix4fv(glGetUniformLocation(shaderID, "proj"), 1, GL_FALSE, &proj[0][0]);

    // Result text
    std::string resultText = won ? "YOU WON" : "YOU DIED";
    glm::vec3 resultColor = won ? glm::vec3(0.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.0f, 0.0f);

    float textX = screenW / 2.0f - resultText.length() * 8.0f;
    float textY = 100.0f;

    glUniform3fv(glGetUniformLocation(shaderID, "color"), 1, &resultColor[0]);
    for (char c : resultText) {
        drawCharacter(c, textX, textY, 1.8f, resultColor, shaderID, VAO, VBO, proj);
        textX += 16.0f;
    }

    // Score display
    std::stringstream ss;
    ss << "SCORE:" << finalScore;
    std::string scoreStr = ss.str();

    textX = screenW / 2.0f - scoreStr.length() * 6.0f;
    textY = 200.0f;
    glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 1.0f, 0.0f);
    for (char c : scoreStr) {
        drawCharacter(c, textX, textY, 1.2f, glm::vec3(1,1,0), shaderID, VAO, VBO, proj);
        textX += 12.0f;
    }

    glEnable(GL_DEPTH_TEST);
}

glm::vec3 TextRenderer::getCharColor(char c) {
    return glm::vec3(1.0f);
}