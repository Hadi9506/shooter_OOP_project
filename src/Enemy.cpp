#include "Enemy.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <time.h>

static auto lastEnemyDebug = std::chrono::steady_clock::now();

// ===================== ENEMY CONSTRUCTOR =====================
Enemy::Enemy(glm::vec3 pos, glm::vec3 col)
    : position(pos), color(col), health(100.0f), alive(true),
      patrolTarget(pos), patrolTimer(0.0f), speed(3.0f), chasing(false), attackTimer(0.0f) {}

void Enemy::takeDamage(float damage) {
    health -= damage;
    if (health <= 0) {
        alive = false;
    }
}

void Enemy::update(float deltaTime, glm::vec3 playerPos) {
    // Placeholder - handled by EnemyManager
}

void Enemy::render(unsigned int cubeVAO, glm::mat4 VP, unsigned int shaderID) {
    // Placeholder - handled by EnemyManager
}

// ===================== ENEMY MANAGER CONSTRUCTOR =====================
EnemyManager::EnemyManager()
    : attackCooldown(1.0f), attackDamage(15.0f), attackRange(2.0f) {}

EnemyManager::~EnemyManager() {
    enemies.clear();
}

// ===================== SPAWN ENEMY =====================
// ✅ FIXED: Removed 'const', now matches header
void EnemyManager::spawn(glm::vec3 pos, glm::vec3 col) {
    enemies.push_back(Enemy(pos, col));
}

// ===================== UPDATE ENEMIES =====================
// ✅ FIXED: Removed 'const', changed parameter types to match header
void EnemyManager::update(float deltaTime, glm::vec3 playerPos) {
    float limit = 20.0f; // world half-size

    for (auto& e : enemies) {
        if (!e.alive) continue;
        
        float distToPlayer = glm::distance(e.position, playerPos);

        // CHASE PLAYER
        if (distToPlayer < 18.0f) {
            e.chasing = true;
            glm::vec3 dir = glm::normalize(playerPos - e.position);
            e.position += dir * e.speed * deltaTime;
        }
        else {
            // PATROL MODE
            e.chasing = false;
            e.patrolTimer += deltaTime;

            // PICK NEW RANDOM POINT IN 40×40 WORLD
            if (glm::distance(e.position, e.patrolTarget) < 0.5f || e.patrolTimer > 5.0f) {
                e.patrolTarget = glm::vec3(
                    -limit + (rand() % int(limit*200)) / 100.0f,
                    1.8f,
                    -limit + (rand() % int(limit*200)) / 100.0f
                );
                e.patrolTimer = 0.0f;
            }

            // Move toward the patrol point
            glm::vec3 dir = glm::normalize(e.patrolTarget - e.position);
            e.position += dir * e.speed * deltaTime * 0.7f;
        }

        // KEEP ENEMIES INSIDE WORLD
        if (e.position.x < -limit) e.position.x = -limit;
        if (e.position.x >  limit) e.position.x =  limit;
        if (e.position.z < -limit) e.position.z = -limit;
        if (e.position.z >  limit) e.position.z =  limit;

        e.position.y = 1.8f; // fixed height
    }
}

// ===================== RENDER ENEMIES =====================
// ✅ FIXED: Removed 'const', changed glm::mat4 to const reference
void EnemyManager::render(unsigned int VAO, glm::mat4 VP, unsigned int shaderID) {
    glBindVertexArray(VAO);

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<float>(now - lastEnemyDebug).count() > 2.0f) {
        std::cout << "[DEBUG] Rendering " << enemies.size() << " enemies\n";
        lastEnemyDebug = now;
    }

    for (const auto& e : enemies) {
        if (!e.alive) continue;
        
        // --- Render Enemy Body ---
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, e.position);
        model = glm::scale(model, glm::vec3(1.5f));
        glm::mat4 mvp = VP * model;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
        glUniform3fv(glGetUniformLocation(shaderID, "uColor"), 1, &e.color[0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // --- Render Health Bar ---
        float healthRatio = e.health / 100.0f;
        if (healthRatio > 0) {
            glm::mat4 barModel = glm::translate(model, glm::vec3(0, 1.2f, 0));
            barModel = glm::scale(barModel, glm::vec3(healthRatio * 1.0f, 0.15f, 0.15f));

            glm::mat4 barMVP = VP * barModel;
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &barMVP[0][0]);
            glUniform3f(glGetUniformLocation(shaderID, "uColor"), 1.0f, 0.0f, 0.0f);
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }
    glBindVertexArray(0);
}

// ===================== HIT TEST (RAY-AABB) =====================
// ✅ NEW: Implemented missing hitTest function
bool EnemyManager::hitTest(const glm::vec3& orig, const glm::vec3& dir, int& outID, float& outDist) {
    outDist = 1e9f;
    int best = -1;
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (!enemies[i].alive) continue;
        
        const Enemy& e = enemies[i];
        glm::vec3 min = e.position - glm::vec3(0.75f);
        glm::vec3 max = e.position + glm::vec3(0.75f);

        // Ray-AABB intersection
        float tmin = (min.x - orig.x) / (dir.x != 0 ? dir.x : 1e-6f);
        float tmax = (max.x - orig.x) / (dir.x != 0 ? dir.x : 1e-6f);
        if (tmin > tmax) std::swap(tmin, tmax);

        float ty_min = (min.y - orig.y) / (dir.y != 0 ? dir.y : 1e-6f);
        float ty_max = (max.y - orig.y) / (dir.y != 0 ? dir.y : 1e-6f);
        if (ty_min > ty_max) std::swap(ty_min, ty_max);

        if (tmin > ty_max || ty_min > tmax) continue;
        if (ty_min > tmin) tmin = ty_min;
        if (ty_max < tmax) tmax = ty_max;

        float tz_min = (min.z - orig.z) / (dir.z != 0 ? dir.z : 1e-6f);
        float tz_max = (max.z - orig.z) / (dir.z != 0 ? dir.z : 1e-6f);
        if (tz_min > tz_max) std::swap(tz_min, tz_max);

        if (tmin > tz_max || tz_min > tmax) continue;
        if (tz_min > tmin) tmin = tz_min;

        if (tmin > 0 && tmin < outDist) {
            outDist = tmin;
            best = (int)i;
        }
    }
    
    if (best != -1) {
        outID = best;
        return true;
    }
    return false;
}

// ===================== ATTACK PLAYER =====================
void EnemyManager::attackPlayer(glm::vec3 playerPos, int& playerHealth, float deltaTime) {
    for (auto& e : enemies) {
        if (!e.alive) continue;
        
        // ✅ FIXED: Changed e.pos to e.position
        float dist = glm::distance(e.position, playerPos);
        const float MELEE_RANGE = 1.8f;
        const float ATTACK_COOLDOWN = 1.0f;

        // Start cooldown timer when in range
        if (dist < MELEE_RANGE) {
            e.attackTimer += deltaTime;
            if (e.attackTimer >= ATTACK_COOLDOWN) {
                int damage = 15;
                playerHealth -= damage;
                std::cout << "Enemy hit you! -" << damage << " HP (Now: " << playerHealth << ")\n";
                e.attackTimer = 0.0f;
            }
        } else {
            e.attackTimer = 0.0f;
        }
    }

    if (playerHealth <= 0) {
        playerHealth = 0;
        std::cout << "=== GAME OVER ===\n";
    }
}

// ===================== CLEAR ALL ENEMIES =====================
void EnemyManager::clear() {
    enemies.clear();
    std::cout << "All enemies cleared\n";
}

// ===================== GET ALL ENEMIES =====================
std::vector<Enemy>& EnemyManager::getAllEnemies() {
    return enemies;
}

// ===================== GET ENEMY COUNT =====================
int EnemyManager::getEnemyCount() const {
    return enemies.size();
}
