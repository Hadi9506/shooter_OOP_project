#pragma once
#include <glm/glm.hpp>
#include <vector>

class Enemy {
    public:
    glm::vec3 pos;
    glm::vec3 color;
    float health = 100.0f;
    float speed = 2.0f;
    float attackTimer = 0.0f;
    glm::vec3 patrolTarget = glm::vec3(0);
    float patrolTimer = 0.0f;
    bool chasing = false;
};

class EnemyManager {
public:
    std::vector<Enemy> enemies;

    void spawn(const glm::vec3& pos, const glm::vec3& col);
    void render(unsigned int VAO, const glm::mat4& VP, unsigned int shaderID) const;
    void update(float deltaTime, const glm::vec3& playerPos);  
    void attackPlayer(glm::vec3 playerPos, int& playerHealth, float deltaTime);
    bool hitTest(const glm::vec3& orig, const glm::vec3& dir, int& outID, float& outDist);
};