#include "Item.h"
#include <iostream>

// Array of item colors for respawning
static glm::vec3 colors[] = {
    glm::vec3(1, 0.3, 0.3),   // Red
    glm::vec3(0.3, 1, 0.3),   // Green
    glm::vec3(0.3, 0.3, 1),   // Blue
    glm::vec3(1, 1, 0.3)      // Yellow
};

// Attempt to pick up item by ID
bool Item::pickUp(World& world, int itemID) {
    for (auto it = world.cubes.begin(); it != world.cubes.end(); ++it) {
        if (it->isItem && it->id == itemID) {
            std::cout << "Item " << itemID << " picked up!\n";
            world.cubes.erase(it);
            return true;
        }
    }
    return false;
}

// Respawn an item at specific position
void Item::respawn(World& world, int itemID, const glm::vec3& pos) {
    if (itemID < 1 || itemID > 4) return;
    world.cubes.push_back({pos, glm::vec3(1), colors[itemID - 1], true, itemID});
    std::cout << "Item " << itemID << " respawned at position\n";
}