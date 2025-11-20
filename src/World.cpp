#include "World.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

static auto lastDebugTime = std::chrono::steady_clock::now();

void World::generate() {
    float W = 40.0f;   // world width
    float H = 40.0f;   // world height

    // FLOOR & CEILING
    cubes.push_back({glm::vec3(0,0,0),   glm::vec3(W, 1, H), glm::vec3(0.7f,0.7f,0.9f), false,0});
    cubes.push_back({glm::vec3(0,10,0),  glm::vec3(W, 1, H), glm::vec3(0.6f,0.9f,1.0f), false,0});

    // WALLS AROUND 40×40
    cubes.push_back({glm::vec3(-W/2,5,0), glm::vec3(1,10,H), glm::vec3(0.8f), false,0});
    cubes.push_back({glm::vec3( W/2,5,0), glm::vec3(1,10,H), glm::vec3(0.8f), false,0});
    cubes.push_back({glm::vec3(0,5,-H/2), glm::vec3(W,10,1), glm::vec3(0.6f,0.6f,1.0f), false,0});
    cubes.push_back({glm::vec3(0,5, H/2), glm::vec3(W,10,1), glm::vec3(0.6f,0.6f,1.0f), false,0});

    // GLOWING ITEMS (spread across world)
    cubes.push_back({glm::vec3(-10,1.5,-10), glm::vec3(0.8f), glm::vec3(1,0.3,0.3), true,1});
    cubes.push_back({glm::vec3( 10,1.5,-10), glm::vec3(0.8f), glm::vec3(0.3,1,0.3), true,2});
    cubes.push_back({glm::vec3( 10,1.5, 10), glm::vec3(0.8f), glm::vec3(0.3,0.3,1), true,3});
    cubes.push_back({glm::vec3(-10,1.5, 10), glm::vec3(0.8f), glm::vec3(1,1,0.3), true,4});
}

void World::render(unsigned int VAO, const glm::mat4& VP, unsigned int shaderID) const {
    glBindVertexArray(VAO);
    
    // Debug every 2 seconds only
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<float>(now - lastDebugTime).count() > 2.0f) {
        std::cout << "[DEBUG] Rendering " << cubes.size() << " world objects\n";
        lastDebugTime = now;
    }
    
    for (const auto& c : cubes) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, c.pos);
        model = glm::scale(model, c.size);
        glm::mat4 mvp = VP * model;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
        glUniform3fv(glGetUniformLocation(shaderID, "uColor"), 1, &c.color[0]);  // ← "uColor" NOT "objectColor"

        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}