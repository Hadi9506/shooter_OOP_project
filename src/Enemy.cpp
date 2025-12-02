#include "Enemy.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <GLFW/glfw3.h>
#include <time.h>
//Commit by Haider
static auto lastEnemyDebug = std::chrono::steady_clock::now();

void EnemyManager::spawn(const glm::vec3& pos, const glm::vec3& col) {
    enemies.push_back({pos, col, 100.0f, 2.0f});
}

//Update enemy AI
void EnemyManager::update(float deltaTime, const glm::vec3& playerPos) {
    float limit = 20.0f; // world half-size

    for (auto& e : enemies) {
        float distToPlayer = glm::distance(e.pos, playerPos);

        // CHASE PLAYER
        if (distToPlayer < 18.0f) {
            e.chasing = true;
            glm::vec3 dir = glm::normalize(playerPos - e.pos);
            e.pos += dir * e.speed * deltaTime;
        }
        else {
            // PATROL MODE
            e.chasing = false;
            e.patrolTimer += deltaTime;

            // PICK NEW RANDOM POINT IN 40×40 WORLD
            if (glm::distance(e.pos, e.patrolTarget) < 0.5f || e.patrolTimer > 5.0f) {
                e.patrolTarget = glm::vec3(
                    -limit + (rand() % int(limit*200)) / 100.0f, // -20 to +20
                    1.8f,
                    -limit + (rand() % int(limit*200)) / 100.0f
                );
                e.patrolTimer = 0.0f;
            }

            // Move toward the patrol point
            glm::vec3 dir = glm::normalize(e.patrolTarget - e.pos);
            e.pos += dir * e.speed * deltaTime * 0.7f;
        }

        // KEEP ENEMIES INSIDE WORLD
        if (e.pos.x < -limit) e.pos.x = -limit;
        if (e.pos.x >  limit) e.pos.x =  limit;
        if (e.pos.z < -limit) e.pos.z = -limit;
        if (e.pos.z >  limit) e.pos.z =  limit;

        e.pos.y = 1.8f; // fixed height
    }
}

void EnemyManager::render(unsigned int VAO, const glm::mat4& VP, unsigned int shaderID) const {
    glBindVertexArray(VAO);

    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<float>(now - lastEnemyDebug).count() > 2.0f) {
        std::cout << "[DEBUG] Rendering " << enemies.size() << " enemies\n";
        lastEnemyDebug = now;
    }

    for (const auto& e : enemies) {
        // --- Render Enemy Body ---
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, e.pos);
        model = glm::scale(model, glm::vec3(1.5f));
        glm::mat4 mvp = VP * model;

        glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &mvp[0][0]);
        glUniform3fv(glGetUniformLocation(shaderID, "uColor"), 1, &e.color[0]);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // --- Render Health Bar ---
        float healthRatio = e.health / 100.0f;
        if (healthRatio > 0) {
            glm::mat4 barModel = glm::translate(model, glm::vec3(0, 1.2f, 0));  // above head
            barModel = glm::scale(barModel, glm::vec3(healthRatio * 1.0f, 0.15f, 0.15f));

            glm::mat4 barMVP = VP * barModel;
            glUniformMatrix4fv(glGetUniformLocation(shaderID, "uMVP"), 1, GL_FALSE, &barMVP[0][0]);
            glUniform3f(glGetUniformLocation(shaderID, "uColor"), 1.0f, 0.0f, 0.0f);  // RED
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
    }
    glBindVertexArray(0);
}
// ---- Ray-AABB (enemy is 1x1x1 cube) ----
bool EnemyManager::hitTest(const glm::vec3& orig, const glm::vec3& dir, int& outID, float& outDist) {
    outDist = 1e9f;
    int best = -1;
    for (size_t i = 0; i < enemies.size(); ++i) {
        const Enemy& e = enemies[i];
        glm::vec3 min = e.pos - glm::vec3(0.5f);
        glm::vec3 max = e.pos + glm::vec3(0.5f);

        float tmin = (min.x - orig.x) / dir.x;
        float tmax = (max.x - orig.x) / dir.x;
        if (tmin > tmax) std::swap(tmin, tmax);

        float ty_min = (min.y - orig.y) / dir.y;
        float ty_max = (max.y - orig.y) / dir.y;
        if (ty_min > ty_max) std::swap(ty_min, ty_max);

        if (tmin > ty_max || ty_min > tmax) continue;
        if (ty_min > tmin) tmin = ty_min;
        if (ty_max < tmax) tmax = ty_max;

        float tz_min = (min.z - orig.z) / dir.z;
        float tz_max = (max.z - orig.z) / dir.z;
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

// EnemyManager.cpp
void EnemyManager::attackPlayer(glm::vec3 playerPos, int& playerHealth, float deltaTime) {
    for (auto& e : enemies) {
        float dist = glm::distance(e.pos, playerPos);
        const float MELEE_RANGE = 1.8f;     // Enemy size 1.5 + player ~0.3
        const float ATTACK_COOLDOWN = 1.0f; // Attack every 1 second

        // Start cooldown timer when in range
        if (dist < MELEE_RANGE) {
            e.attackTimer += deltaTime;
            if (e.attackTimer >= ATTACK_COOLDOWN) {
                int damage = 15;
                playerHealth -= damage;
                std::cout << "Enemy hit you! -" << damage << " HP (Now: " << playerHealth << ")\n";
                e.attackTimer = 0.0f;  // Reset
            }
        } else {
            e.attackTimer = 0.0f;  // Reset if out of range
        }
    }

    // GAME OVER
    if (playerHealth <= 0) {
        playerHealth = 0;
        std::cout << "=== GAME OVER ===\n";
        // You can add screen later
    }
}

// ===================== CLEAR ALL ENEMIES =====================
void EnemyManager::clear() {
    enemies.clear();
    std::cout << "All enemies cleared\n";
}
