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
    
    // ✅ NEW: Missing members for AI
    glm::vec3 patrolTarget;
    float patrolTimer;
    float speed;
    bool chasing;
    float attackTimer;

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

    EnemyManager();
    ~EnemyManager();
    
    // ✅ FIXED: Removed 'const' and fixed parameter types
    void spawn(glm::vec3 position, glm::vec3 color);
    void clear();
    void update(float deltaTime, glm::vec3 playerPos);
    void render(unsigned int cubeVAO, glm::mat4 VP, unsigned int shaderID);
    void attackPlayer(glm::vec3 playerPos, int& playerHealth, float deltaTime);
    
    // ✅ NEW: Added missing hitTest function
    bool hitTest(const glm::vec3& orig, const glm::vec3& dir, int& outID, float& outDist);
    
    std::vector<Enemy>& getAllEnemies();
    int getEnemyCount() const;
};