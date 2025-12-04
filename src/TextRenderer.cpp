#include "TextRenderer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include "WeaponSystem.h"

// Declare the external variables
extern int currentAmmo;
extern int reserveMags;
extern int partialMagAmmo;
extern WeaponSystem ws;

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

    unsigned int f = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(f, 1, &fs, nullptr);
    glCompileShader(f);

    shaderID = glCreateProgram();
    glAttachShader(shaderID, v);
    glAttachShader(shaderID, f);
    glLinkProgram(shaderID);
    glDeleteShader(v);
    glDeleteShader(f);

    // VAO / VBO
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
void TextRenderer::RenderHUD(int health, int score, int /*unused*/, int /*unused*/, unsigned int w, unsigned int h) {
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

    // === 2. WEAPON AMMO BARS (3 types) ===
    float ammoStartX = margin + 230;
    float barW = 120.0f;
    float barH2 = 18.0f;
    float spacingY = 22.0f;

    for (int i = 0; i < 3; ++i) {
        const auto& reserve = ws.weapons[i];
        float currentY = barY - i * spacingY;

        // Background
        float bgVerts[] = {
            ammoStartX - 2, currentY + 2,
            ammoStartX + barW + 2, currentY + 2,
            ammoStartX - 2, currentY - barH2 - 2,
            ammoStartX + barW + 2, currentY - barH2 - 2
        };
        glUniform3f(glGetUniformLocation(shaderID, "color"), 0.0f, 0.0f, 0.0f);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(bgVerts), bgVerts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // Current ammo fill
        float pct = (float)reserve.currentAmmo / reserve.type->getMagSize();
        glm::vec3 col = reserve.type->getTracerColor();
        float fillVerts[] = {
            ammoStartX, currentY,
            ammoStartX + barW * pct, currentY,
            ammoStartX, currentY - barH2,
            ammoStartX + barW * pct, currentY - barH2
        };
        glUniform3fv(glGetUniformLocation(shaderID, "color"), 1, &col[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(fillVerts), fillVerts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        // === SELECTION INDICATOR (black bar on top of current weapon) ===
        if (i == ws.currentWeapon) {
            float selVerts[] = {
                ammoStartX, currentY + barH2 + 3,
                ammoStartX + barW, currentY + barH2 + 3,
                ammoStartX, currentY + barH2 + 7,
                ammoStartX + barW, currentY + barH2 + 7
            };
            glUniform3f(glGetUniformLocation(shaderID, "color"), 0.0f, 0.0f, 0.0f);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(selVerts), selVerts);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }

        // === RESERVE MAGS (white blocks) ===
        float blockX = ammoStartX + barW + 10;
        float blockW = 12.0f;
        glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 1.0f, 1.0f);
        for (int m = 0; m < reserve.reservedMags && m < 8; ++m) {
            float x = blockX + m * (blockW + 4);
            float blockVerts[] = {
                x, currentY,
                x + blockW, currentY,
                x, currentY - barH2,
                x + blockW, currentY - barH2
            };
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(blockVerts), blockVerts);
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }

    // === 3. SCORE TEXT ===
    std::string scoreStr = "SCORE: " + std::to_string(score);
    float textX = ammoStartX + barW + 150, textY = barY - barH + 8;
    float charW = 12.0f, charH = 16.0f;
    glUniform3f(glGetUniformLocation(shaderID, "color"), 1.0f, 1.0f, 1.0f);

    for (char c : scoreStr) {
        if (c == ' ') { textX += charW; continue; }
        float verts[] = {
            textX, textY,
            textX + charW, textY,
            textX, textY + charH,
            textX + charW, textY + charH
        };
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(verts), verts);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        textX += charW + 2;
    }

    glEnable(GL_DEPTH_TEST);
}
