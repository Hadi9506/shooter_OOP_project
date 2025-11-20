#include "Shooter.h"
#include <iostream>
#include <algorithm>

extern int playerHealth;
extern int score;
extern int ammo;

bool Shooter::rayAABB(const glm::vec3& orig, const glm::vec3& dir,
                      const Cube& cube, float& t) {
    glm::vec3 min = cube.pos - cube.size * 0.5f;
    glm::vec3 max = cube.pos + cube.size * 0.5f;

    float tmin = (min.x - orig.x) / dir.x;
    float tmax = (max.x - orig.x) / dir.x;
    if (tmin > tmax) std::swap(tmin, tmax);

    float ty_min = (min.y - orig.y) / dir.y;
    float ty_max = (max.y - orig.y) / dir.y;
    if (ty_min > ty_max) std::swap(ty_min, ty_max);

    if (tmin > ty_max || ty_min > tmax) return false;
    if (ty_min > tmin) tmin = ty_min;
    if (ty_max < tmax) tmax = ty_max;

    float tz_min = (min.z - orig.z) / dir.z;
    float tz_max = (max.z - orig.z) / dir.z;
    if (tz_min > tz_max) std::swap(tz_min, tz_max);

    if (tmin > tz_max || tz_min > tmax) return false;
    if (tz_min > tmin) tmin = tz_min;

    t = tmin;
    return tmin > 0;
}
void Shooter::fire(const Camera& cam, World& world, EnemyManager& enemies) {
    glm::vec3 rayOrigin = cam.position;
    glm::vec3 rayDir    = cam.front;

    // ---- Check Items first ----
    float closest = 1e9f;
    int   hitItem = -1;
    for (size_t i = 0; i < world.cubes.size(); ++i) {
        const Cube& cube = world.cubes[i];
        if (!cube.isItem) continue;
        float t;
        if (rayAABB(rayOrigin, rayDir, cube, t) && t < closest) {
            closest = t;
            hitItem = (int)i;
        }
    }
    if (hitItem != -1) {
        const Cube& c = world.cubes[hitItem];       // 
        Item::pickUp(world, c.id);
        score += 5;                                 // global score
        return;
    }

    // ---- Check Enemies ----
    int   enemyID;
    float enemyDist;
    if (enemies.hitTest(rayOrigin, rayDir, enemyID, enemyDist)) {
        if (ammo > 0) {
            ammo--;
            score += 10;
            enemies.enemies[enemyID].health -= 34.0f;
        if (enemies.enemies[enemyID].health <= 0) {
            score += 50;
            enemies.enemies.erase(enemies.enemies.begin() + enemyID);
        }}
        else ammo =0;
        return;
    }

    if (ammo > 0) ammo--;
    else ammo = 0;
}