#include "Enemy.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <time.h>

// Static timer for debug output
static auto lastEnemyDebug = std::chrono::steady_clock::now();

// Constructor: initialize enemy with position and color
Enemy::Enemy(glm::vec3 pos, glm::vec3 col)
    : position(pos), color(col), health(100.0f), alive(true),
      patrolTarget(pos), patrolTimer(0.0f), speed(3.0f), chasing(false), attackTimer(0.0f) {}

// Reduce enemy health and mark as dead if health reaches 0
void Enemy::takeDamage(float damage) {
    health -= damage;
    if (health <= 0) {
        alive = false;
    }
}

// Update enemy state (placeholder - handled by manager)
void Enemy::update(float deltaTime, glm::vec3 playerPos) {
    // Actual logic in EnemyManager
}

// Render enemy (placeholder - handled by manager)
void Enemy::render(unsigned int cubeVAO, glm::mat4 VP, unsigned int shaderID) {
    // Actual rendering in EnemyManager
}

// Constructor: initialize enemy manager with default attack values
EnemyManager::EnemyManager()
    : attackCooldown(1.0f), attackDamage(15.0f), attackRange(2.0f) {}

// Destructor: clean up enemies
EnemyManager::~EnemyManager() {
    enemies.clear();
}

// Add a new enemy to the game world
void EnemyManager::spawn(glm::vec3 pos, glm::vec3 col) {
    enemies.push_back(Enemy(pos, col));
}

// Update all enemies each frame
// Handles AI behavior including patrol and chase
void EnemyManager::update(float deltaTime, glm::vec3 playerPos) {
    float limit = 20.0f;  // World boundary

    for (auto& e : enemies) {
        if (!e.alive) continue;  // Skip dead enemies
        
        float distToPlayer = glm::distance(e.position, playerPos);

        // If player is close, chase them
        if (distToPlayer < 18.0f) {
            e.chasing = true;
            glm::vec3 dir = glm::normalize(playerPos - e.position);
            e.position += dir * e.speed * deltaTime;
        }
        // Otherwise patrol randomly
        else {
            e.chasing = false;
            e.patrolTimer += deltaTime;

            // Pick new patrol target every 5 seconds or when reached
            if (glm::distance(e.position, e.patrolTarget) < 0.5f || e.patrolTimer > 5.0f) {
                e.patrolTarget = glm::vec3(
                    -limit + (rand() % int(limit * 200)) / 100.0f,
                    1.8f,
                    -limit + (rand() % int(limit * 200)) / 100.0f
                );
                e.patrolTimer = 0.0f;
            }

            // Move toward patrol target at reduced speed
            glm::vec3 dir = glm::normalize(e.patrolTarget - e.position);
            e.position += dir * e.speed * deltaTime * 0.7f;
        }

        // Keep enemies within world bounds
        if (e.position.x < -limit) e.position.x = -limit;
        if (e.position.x > limit) e.position.x = limit;
        if (e.position.z < -limit) e.position.z = -limit;
        if (e.position.z > limit) e.position.z = limit;

        // Keep enemy at fixed height
        e.position.y = 1.8f;
    }
}

// Render all living enemies with health bars
void EnemyManager::render(unsigned int VAO, glm::mat4 VP, unsigned int shaderID) {
    glBindVertexArray(VAO);

    // Debug output every 2 seconds
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<float>(now - lastEnemyDebug).count() > 2.0f) {
        std::cout << "[DEBUG] Rendering " << enemies.size() << " enemies\n";
        lastEnemyDebug = now;
    }

    for (const auto& e : enemies) {
        if (!e.alive) continue;
        
        // Render enemy body as cube
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, e.position);
        model = glm::scale(model, glm::vec3(1.5f));
        glm::mat4 mvp = VP * model;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
        glUniform3fv(glGetUniformLocation(shaderID, "uColor"), 1, &e.color[0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Render health bar above enemy
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

// Ray-AABB intersection test for shooting enemies
bool EnemyManager::hitTest(const glm::vec3& orig, const glm::vec3& dir, int& outID, float& outDist) {
    outDist = 1e9f;
    int best = -1;
    
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (!enemies[i].alive) continue;
        
        const Enemy& e = enemies[i];
        // Create bounding box around enemy
        glm::vec3 min = e.position - glm::vec3(0.75f);
        glm::vec3 max = e.position + glm::vec3(0.75f);

        // Ray-AABB intersection on X axis
        float tmin = (min.x - orig.x) / (dir.x != 0 ? dir.x : 1e-6f);
        float tmax = (max.x - orig.x) / (dir.x != 0 ? dir.x : 1e-6f);
        if (tmin > tmax) std::swap(tmin, tmax);

        // Ray-AABB intersection on Y axis
        float ty_min = (min.y - orig.y) / (dir.y != 0 ? dir.y : 1e-6f);
        float ty_max = (max.y - orig.y) / (dir.y != 0 ? dir.y : 1e-6f);
        if (ty_min > ty_max) std::swap(ty_min, ty_max);

        if (tmin > ty_max || ty_min > tmax) continue;
        if (ty_min > tmin) tmin = ty_min;
        if (ty_max < tmax) tmax = ty_max;

        // Ray-AABB intersection on Z axis
        float tz_min = (min.z - orig.z) / (dir.z != 0 ? dir.z : 1e-6f);
        float tz_max = (max.z - orig.z) / (dir.z != 0 ? dir.z : 1e-6f);
        if (tz_min > tz_max) std::swap(tz_min, tz_max);

        if (tmin > tz_max || tz_min > tmax) continue;
        if (tz_min > tmin) tmin = tz_min;

        // Find closest hit enemy
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

// Apply damage from enemies to player
void EnemyManager::attackPlayer(glm::vec3 playerPos, int& playerHealth, float deltaTime) {
    for (auto& e : enemies) {
        if (!e.alive) continue;
        
        // Calculate distance to player
        float dist = glm::distance(e.position, playerPos);
        const float MELEE_RANGE = 1.8f;
        const float ATTACK_COOLDOWN = 1.0f;

        // If player is in melee range, attack
        if (dist < MELEE_RANGE) {
            e.attackTimer += deltaTime;
            if (e.attackTimer >= ATTACK_COOLDOWN) {
                int damage = 15;
                playerHealth -= damage;
                std::cout << "Enemy hit you! -" << damage << " HP (Now: " << playerHealth << ")\n";
                e.attackTimer = 0.0f;  // Reset cooldown
            }
        } 
        // Otherwise reset cooldown
        else {
            e.attackTimer = 0.0f;
        }
    }

    // Clamp health to 0
    if (playerHealth <= 0) {
        playerHealth = 0;
        std::cout << "=== GAME OVER ===\n";
    }
}

// Remove all enemies from the game
void EnemyManager::clear() {
    enemies.clear();
    std::cout << "All enemies cleared\n";
}

// Return reference to enemy vector
std::vector<Enemy>& EnemyManager::getAllEnemies() {
    return enemies;
}

// Return number of enemies
int EnemyManager::getEnemyCount() const {
    return enemies.size();
}
