#include "World.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>

// Static timer for debug output to avoid spam
static auto lastDebugTime = std::chrono::steady_clock::now();

// Generate the initial game world with all geometry
void World::generate() {
    // Define world boundaries
    float W = 40.0f;   // World width and depth
    float H = 40.0f;   // World height

    // Create floor at ground level
    // Large flat surface for player to walk on
    cubes.push_back({glm::vec3(0, 0, 0), glm::vec3(W, 1, H), glm::vec3(0.7f, 0.7f, 0.9f), false, 0});
    
    // Create ceiling at top of world
    // Prevents player from flying away
    cubes.push_back({glm::vec3(0, 10, 0), glm::vec3(W, 1, H), glm::vec3(0.6f, 0.9f, 1.0f), false, 0});

    // Create boundary walls around the world
    // Left wall (negative X)
    cubes.push_back({glm::vec3(-W/2, 5, 0), glm::vec3(1, 10, H), glm::vec3(0.8f), false, 0});
    
    // Right wall (positive X)
    cubes.push_back({glm::vec3(W/2, 5, 0), glm::vec3(1, 10, H), glm::vec3(0.8f), false, 0});
    
    // Back wall (negative Z)
    cubes.push_back({glm::vec3(0, 5, -H/2), glm::vec3(W, 10, 1), glm::vec3(0.6f, 0.6f, 1.0f), false, 0});
    
    // Front wall (positive Z)
    cubes.push_back({glm::vec3(0, 5, H/2), glm::vec3(W, 10, 1), glm::vec3(0.6f, 0.6f, 1.0f), false, 0});

    // Create collectible items spread across the world
    // Each item has unique ID and color for identification
    // Red item - top left
    cubes.push_back({glm::vec3(-10, 1.5, -10), glm::vec3(0.8f), glm::vec3(1, 0.3, 0.3), true, 1});
    
    // Green item - top right
    cubes.push_back({glm::vec3(10, 1.5, -10), glm::vec3(0.8f), glm::vec3(0.3, 1, 0.3), true, 2});
    
    // Blue item - bottom right
    cubes.push_back({glm::vec3(10, 1.5, 10), glm::vec3(0.8f), glm::vec3(0.3, 0.3, 1), true, 3});
    
    // Yellow item - bottom left
    cubes.push_back({glm::vec3(-10, 1.5, 10), glm::vec3(0.8f), glm::vec3(1, 1, 0.3), true, 4});
}

// Clear all items and respawn them at their original positions
void World::regenerateItems() {
    // Remove all items from the world
    // Items are identified by the isItem flag
    auto it = cubes.begin();
    while (it != cubes.end()) {
        if (it->isItem) {
            it = cubes.erase(it);
        } else {
            ++it;
        }
    }
    
    // Respawn all items at their fixed starting locations
    // Red item - top left corner
    cubes.push_back({glm::vec3(-10, 1.5, -10), glm::vec3(0.8f), glm::vec3(1, 0.3, 0.3), true, 1});
    
    // Green item - top right corner
    cubes.push_back({glm::vec3(10, 1.5, -10), glm::vec3(0.8f), glm::vec3(0.3, 1, 0.3), true, 2});
    
    // Blue item - bottom right corner
    cubes.push_back({glm::vec3(10, 1.5, 10), glm::vec3(0.8f), glm::vec3(0.3, 0.3, 1), true, 3});
    
    // Yellow item - bottom left corner
    cubes.push_back({glm::vec3(-10, 1.5, 10), glm::vec3(0.8f), glm::vec3(1, 1, 0.3), true, 4});
    
    std::cout << "Items regenerated\n";
}

// Render all cubes in the world using the provided vertex array
void World::render(unsigned int VAO, const glm::mat4& VP, unsigned int shaderID) const {
    // Bind the cube vertex array for rendering
    glBindVertexArray(VAO);
    
    // Print debug info every 2 seconds to avoid console spam
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<float>(now - lastDebugTime).count() > 2.0f) {
        std::cout << "[DEBUG] Rendering " << cubes.size() << " world objects\n";
        lastDebugTime = now;
    }
    
    // Render each cube in the world
    for (const auto& c : cubes) {
        // Create model matrix for this cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, c.pos);      // Move to position
        model = glm::scale(model, c.size);         // Scale to size
        
        // Calculate final MVP matrix
        glm::mat4 mvp = VP * model;

        // Pass matrix and color to shader
        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
        glUniform3fv(glGetUniformLocation(shaderID, "uColor"), 1, &c.color[0]);

        // Draw the cube as 36 triangle vertices
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}