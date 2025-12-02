#pragma once
#include "Camera.h"
#include "World.h"
#include "Enemy.h"
#include "Item.h"

// Shooter class handles all weapon firing and collision detection
// Manages ray casting for item pickup and enemy hit detection
class Shooter {
public:
    // Fire weapon from camera position in camera direction
    // Handles ray casting for item collection and enemy damage
    // Returns after checking for collision with items or enemies
    static void fire(const Camera& cam, World& world, EnemyManager& enemies);
    
private:
    // Ray-AABB (Axis-Aligned Bounding Box) collision detection
    // Tests if a ray from origin in direction hits the given cube
    // Sets t to the distance along ray where intersection occurs
    // Returns true if ray intersects with cube, false otherwise
    static bool rayAABB(const glm::vec3& orig, const glm::vec3& dir,
    const Cube& cube, float& t);
};