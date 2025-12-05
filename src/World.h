#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

// Cube structure represents a single 3D cube in the world
// Used for both static geometry (walls, floor) and collectible items
struct Cube {
    glm::vec3 pos;        // Position in world space
    glm::vec3 size;       // Width, height, depth dimensions
    glm::vec3 color;      // RGB color for rendering
    bool isItem;          // True if collectible item, false if wall/static
    int id;               // Unique identifier for items
};

// World class manages all static geometry in the game
// Handles generation and rendering of environment
class World {
public:
    std::vector<Cube> cubes;  // All cubes in the world
    
    // Generate initial world layout with walls, floor, ceiling, and items
    void generate();
    
    // Clear items and respawn them at starting positions
    void regenerateItems();
    
    // Render all cubes using the provided VAO and shader
    void render(unsigned int VAO, const glm::mat4& VP, unsigned int shaderID) const;
    
    // Clear and regenerate the world
    void reset() {
        cubes.clear();
        generate();
    }
};