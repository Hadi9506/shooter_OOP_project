#include "Shooter.h"
#include <iostream>
#include <algorithm>
#include "Tracer.h"
#include "WeaponSystem.h"

extern TracerManager tracerManager;  // Global tracers
extern int playerHealth;
extern int score;
extern int currentAmmo;
extern int reserveMags;
extern WeaponSystem ws;

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
    float hitDist = closest;
    if (hitItem != -1) {
        glm::vec3 hitPos = rayOrigin + rayDir * hitDist;  // ← HIT POINT
        tracerManager.add(rayOrigin, hitPos);             // ← ADD TRACER
        const Cube& c = world.cubes[hitItem];
        Item::pickUp(world, c.id);
        score += 5; // global score
        reserveMags++;
        return;
    }

    // ---- Check Enemies ----
    auto* bullet = ws.getCurrentBullet();
    auto& reserve = ws.getCurrentReserve();

    if (reserve.currentAmmo <= 0) return;

    reserve.currentAmmo--;
    glm::vec3 hitPos = rayOrigin + rayDir * hitDist;
    tracerManager.add(rayOrigin, hitPos);


    int   enemyID;
    float enemyDist;
    if (enemies.hitTest(rayOrigin, rayDir, enemyID, enemyDist)) {
        if (currentAmmo > 0) {
            glm::vec3 hitPos = rayOrigin + rayDir * enemyDist;  // ← HIT POINT
            tracerManager.add(rayOrigin, hitPos);               // ← ADD TRACER
            currentAmmo--;
            score += 10;
            enemies.enemies[enemyID].health -= bullet->getDamage();
        if (enemies.enemies[enemyID].health <= 0) {
            score += 50;
            enemies.enemies.erase(enemies.enemies.begin() + enemyID);
        }}
        else currentAmmo =0;
        return;
    }

    // Missed – short tracer
    hitPos = rayOrigin + rayDir * 50.0f;  // Max range
    tracerManager.add(rayOrigin, hitPos);

    if (currentAmmo > 0) currentAmmo--;
    else currentAmmo = 0;
}
