#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

class Enemy {
public:
    glm::vec3 position;
    glm::vec3 color;
    float health;
    bool alive;

    Enemy(glm::vec3 pos, glm::vec3 col);
    void takeDamage(float damage);
    void update(float deltaTime, glm::vec3 playerPos);
    void render(unsigned int cubeVAO, glm::mat4 VP, unsigned int shaderID);
};

class EnemyManager {
public:
    std::vector<Enemy> enemies;
    float attackCooldown;
    float attackDamage;
    float attackRange;

//public:
    EnemyManager();
    ~EnemyManager();
    
    void spawn (glm::vec3 position, glm::vec3 color) const;
    void clear();  // ✅ NEW: Clear all enemies
    void update(float deltaTime, glm::vec3 playerPos) const;
    void render(unsigned int cubeVAO, glm::mat4 VP, unsigned int shaderID) const;
    void attackPlayer(glm::vec3 playerPos, int& playerHealth, float deltaTime);
    std::vector<Enemy>& getAllEnemies();
    int getEnemyCount() const;
};