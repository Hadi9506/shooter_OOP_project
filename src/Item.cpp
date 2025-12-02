#include "Item.h"
#include <iostream>

// Search for item by ID and remove it from world
bool Item::pickUp(World& world, int itemID) {
    for (auto it = world.cubes.begin(); it != world.cubes.end(); ++it) {
        // Check if this cube is an item and matches the ID
        if (it->isItem && it->id == itemID) {
            std::cout << "Picked up item " << itemID << " (color "
                      << it->color.x << "," << it->color.y << "," << it->color.z << ")\n";
            // Remove the item from world
            world.cubes.erase(it);
            return true;
        }
    }
    return false;
}

// Add item back to world at specified position
void Item::respawn(World& world, int itemID, const glm::vec3& pos) {
    // Predefined colors for each item ID
    glm::vec3 colors[] = {
        glm::vec3(1, 0, 0),   // Red
        glm::vec3(0, 1, 0),   // Green
        glm::vec3(0, 0, 1),   // Blue
        glm::vec3(1, 1, 0)    // Yellow
    };
    
    // Add new item cube to world at position
    world.cubes.push_back({pos, glm::vec3(1), colors[itemID - 1], true, itemID});
}