#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// Camera class handles first-person view and physics
// Manages position, orientation, and movement
class Camera {
public:
    glm::vec3 position;      // Camera position in world space
    glm::vec3 front;         // Direction camera is looking
    glm::vec3 up;            // Up vector for view matrix
    glm::vec3 right;         // Right vector (perpendicular to up)
    glm::vec3 worldUp;       // Global up direction (0,1,0)

    float yaw;               // Horizontal rotation angle
    float pitch;             // Vertical rotation angle
    float speed;             // Movement speed
    float sensitivity;       // Mouse sensitivity for rotation
    float velocityY;         // Vertical velocity for jumping/gravity
    bool isOnGround;         // Whether camera is on ground for jumping
    
    const float GRAVITY;     // Gravity acceleration
    const float JUMP_FORCE;  // Initial jump velocity
    const float GROUND_Y;    // Ground level Y position

    // Constructor with optional starting position
    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f));

    // Get the view matrix for rendering
    glm::mat4 getViewMatrix() const;
    
    // Handle keyboard movement (0=forward, 1=back, 2=left, 3=right)
    void processKeyboard(int direction, float deltaTime);
    
    // Handle mouse look rotation
    void processMouse(float xoffset, float yoffset);
    
    // Apply gravity and collision physics
    void physics(float deltaTime);
    
    // Jump if on ground
    void jump();
    
private:
    // Recalculate front, right, and up vectors from yaw and pitch
    void updateVectors();
};