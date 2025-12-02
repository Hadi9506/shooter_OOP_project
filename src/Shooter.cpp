#include "Shooter.h"
#include <iostream>
#include <algorithm>

// External game state variables
extern int playerHealth;
extern int score;
extern int currentAmmo;
extern int reserveMags;

// Ray-AABB collision test
// Determines if a ray intersects with an axis-aligned bounding box
// Returns true and sets t to intersection distance if hit
bool Shooter::rayAABB(const glm::vec3& orig, const glm::vec3& dir,
                      const Cube& cube, float& t) {
    // Get AABB bounds from cube position and size
    glm::vec3 min = cube.pos - cube.size * 0.5f;
    glm::vec3 max = cube.pos + cube.size * 0.5f;

    // Test ray intersection on X axis
    float tmin = (min.x - orig.x) / dir.x;
    float tmax = (max.x - orig.x) / dir.x;
    if (tmin > tmax) std::swap(tmin, tmax);

    // Test ray intersection on Y axis
    float ty_min = (min.y - orig.y) / dir.y;
    float ty_max = (max.y - orig.y) / dir.y;
    if (ty_min > ty_max) std::swap(ty_min, ty_max);

    // Check if X and Y ranges overlap
    if (tmin > ty_max || ty_min > tmax) return false;
    if (ty_min > tmin) tmin = ty_min;
    if (ty_max < tmax) tmax = ty_max;

    // Test ray intersection on Z axis
    float tz_min = (min.z - orig.z) / dir.z;
    float tz_max = (max.z - orig.z) / dir.z;
    if (tz_min > tz_max) std::swap(tz_min, tz_max);

    // Check if all axes overlap
    if (tmin > tz_max || tz_min > tmax) return false;
    if (tz_min > tmin) tmin = tz_min;

    t = tmin;
    return tmin > 0;
}

// Fire weapon in direction of camera
// Checks for item pickups and enemy hits
void Shooter::fire(const Camera& cam, World& world, EnemyManager& enemies) {
    // Create ray from camera position in camera direction
    glm::vec3 rayOrigin = cam.position;
    glm::vec3 rayDir = cam.front;

    // First check if we hit any items
    float closest = 1e9f;
    int hitItem = -1;
    
    for (size_t i = 0; i < world.cubes.size(); ++i) {
        const Cube& cube = world.cubes[i];
        if (!cube.isItem) continue;  // Skip non-items
        
        // Check ray collision with item
        float t;
        if (rayAABB(rayOrigin, rayDir, cube, t) && t < closest) {
            closest = t;
            hitItem = (int)i;
        }
    }
    
    // If we hit an item, pick it up and add ammo
    if (hitItem != -1) {
        const Cube& c = world.cubes[hitItem];
        Item::pickUp(world, c.id);
        score += 5;  // Small score for collecting items
        reserveMags++;  // Add one magazine to reserves
        return;
    }

    // Check if we hit any enemies
    int enemyID;
    float enemyDist;
    if (enemies.hitTest(rayOrigin, rayDir, enemyID, enemyDist)) {
        std::cout << "HIT ENEMY " << enemyID << " at distance " << enemyDist << "\n";
        
        // Deal damage to the enemy
        enemies.getAllEnemies()[enemyID].takeDamage(25.0f);
        
        // Check if enemy died and add score
        if (!enemies.getAllEnemies()[enemyID].alive) {
            extern int score;
            score += 100;  // Large score for killing enemies
            std::cout << "Enemy killed! Score: " << score << "\n";
        }
    }

    // Consume one bullet from current magazine
    if (currentAmmo > 0) currentAmmo--;
    else currentAmmo = 0;
}