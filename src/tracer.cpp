// Tracer.cpp
#include "Tracer.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

struct LineVertex {
    glm::vec3 position;
};

static unsigned int lineVAO = 0;
static unsigned int lineVBO = 0;

void initLineRenderer() {
    glGenVertexArrays(1, &lineVAO);
    glGenBuffers(1, &lineVBO);

    glBindVertexArray(lineVAO);
    glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(LineVertex) * 2, nullptr, GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(LineVertex), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void TracerManager::add(const glm::vec3& start, const glm::vec3& end) {
    tracers.push_back({start, end});
}

void TracerManager::update(float deltaTime) {
    for (auto it = tracers.begin(); it != tracers.end(); ) {
        it->life -= deltaTime;
        if (it->life <= 0.0f) {
            it = tracers.erase(it);
        } else {
            ++it;
        }
    }
}

void TracerManager::render(unsigned int shaderID, const glm::mat4& projection, const glm::mat4& view) const {
    if (tracers.empty()) return;

    if (lineVAO == 0) {
        initLineRenderer();  // one-time setup
    }

    glUseProgram(shaderID);
    glBindVertexArray(lineVAO);
    glLineWidth(3.0f);

    LineVertex vertices[2];

    for (const auto& t : tracers) {
        float alpha = glm::clamp(t.life / 0.2f, 1.0f, 1.0f);
        glm::vec3 color = glm::vec3(1.0f, 0.9f, 0.3f) * alpha;  // Bright yellow

        // CORRECT MVP: model = identity → just P * V * position
        glm::mat4 model = glm::mat4(0.6f);
        glm::mat4 mvp = projection * view * model;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
        glUniform3fv(glGetUniformLocation(shaderID, "uColor"), 1, &color[0]);

        vertices[0].position = glm::vec3(t.start.x+0.1,t.start.y-0.2,t.start.z-0.1);
        vertices[1].position = t.end;

        glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
        glDrawArrays(GL_LINES, 0, 2);
    }

    glBindVertexArray(0);
    glLineWidth(1.0f);
}