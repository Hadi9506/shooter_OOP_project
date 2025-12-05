#pragma once
#include "World.h"
#include <glm/glm.hpp>

// Item class handles collectible items in the game world
// Items can be picked up by the player for various effects
class Item {
public:
    // Attempt to pick up an item by ID from the world
    static bool pickUp(World& world, int itemID);
    
    // Respawn an item at a specific position
    static void respawn(World& world, int itemID, const glm::vec3& pos);
};