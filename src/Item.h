#pragma once
#include "World.h"

class Item {
public:
    static bool pickUp(World& world, int itemID);
    static void respawn(World& world, int itemID, const glm::vec3& pos);
};