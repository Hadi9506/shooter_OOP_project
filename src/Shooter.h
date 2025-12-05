#pragma once
#include "Camera.h"
#include "World.h"
#include "Enemy.h"
#include "Item.h"

class Shooter {
public:
    static void fire(const Camera& cam, World& world, EnemyManager& enemies);
private:
    static bool rayAABB(const glm::vec3& orig, const glm::vec3& dir, const Cube& cube, float& t);
};
