#pragma once
#include "World.h"

// Item class provides static methods for item management
// Handles pickup and respawn mechanics
class Item {
public:
    // Remove an item from the world by its ID
    // Returns true if item was found and removed
    static bool pickUp(World& world, int itemID);
    
    // Add an item back to the world at specified position
    static void respawn(World& world, int itemID, const glm::vec3& pos);
};