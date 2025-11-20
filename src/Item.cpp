#include "Item.h"
#include <iostream>

bool Item::pickUp(World& world, int itemID) {
    for (auto it = world.cubes.begin(); it != world.cubes.end(); ++it) {
        if (it->isItem && it->id == itemID) {
            std::cout << "Picked up item " << itemID << " (color "
                      << it->color.x << "," << it->color.y << "," << it->color.z << ")\n";
            world.cubes.erase(it);
            return true;
        }
    }
    return false;
}

void Item::respawn(World& world, int itemID, const glm::vec3& pos) {
    glm::vec3 colors[] = {
        glm::vec3(1,0,0), glm::vec3(0,1,0), glm::vec3(0,0,1), glm::vec3(1,1,0)
    };
    world.cubes.push_back({pos, glm::vec3(1), colors[itemID-1], true, itemID});
}