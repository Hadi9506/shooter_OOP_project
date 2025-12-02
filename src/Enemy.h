#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "Shader.h"

// Enemy class represents a single enemy character
// Handles individual enemy state and behavior
class Enemy {
public:
    glm::vec3 position;      // Current world position
    glm::vec3 color;         // Color for rendering
    float health;            // Current health points
    bool alive;              // Whether enemy is still alive
    
    glm::vec3 patrolTarget;  // Target position while patrolling
    float patrolTimer;       // Timer for patrol behavior
    float speed;             // Movement speed
    bool chasing;            // Whether currently chasing player
    float attackTimer;       // Cooldown timer for attacks

    // Constructor with position and color
    Enemy(glm::vec3 pos, glm::vec3 col);
    
    // Apply damage to this enemy
    void takeDamage(float damage);
    
    // Update enemy state each frame (handled by manager)
    void update(float deltaTime, glm::vec3 playerPos);
    
    // Render this enemy (handled by manager)
    void render(unsigned int cubeVAO, glm::mat4 VP, unsigned int shaderID);
};

// EnemyManager class manages all enemies in the game
// Handles spawning, updating, rendering, and combat
class EnemyManager {
public:
    std::vector<Enemy> enemies;    // All enemies in game
    float attackCooldown;          // Time between enemy attacks
    float attackDamage;            // Damage per attack
    float attackRange;             // Distance for melee attack

    EnemyManager();
    ~EnemyManager();
    
    // Spawn a new enemy at position with color
    void spawn(glm::vec3 position, glm::vec3 color);
    
    // Remove all enemies
    void clear();
    
    // Update all enemies each frame
    void update(float deltaTime, glm::vec3 playerPos);
    
    // Render all enemies
    void render(unsigned int cubeVAO, glm::mat4 VP, unsigned int shaderID);
    
    // Apply damage to player from enemies
    void attackPlayer(glm::vec3 playerPos, int& playerHealth, float deltaTime);
    
    // Ray-based hit detection for shooting enemies
    bool hitTest(const glm::vec3& orig, const glm::vec3& dir, int& outID, float& outDist);
    
    // Get reference to all enemies
    std::vector<Enemy>& getAllEnemies();
    
    // Get number of enemies
    int getEnemyCount() const;
};