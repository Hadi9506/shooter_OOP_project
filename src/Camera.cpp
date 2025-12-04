#include "Camera.h"
#include <glm/gtc/constants.hpp>
#include <iostream>

//Haider Commit
// Constructor: initialize camera with position and default orientation
Camera::Camera(glm::vec3 pos) : position(pos), worldUp(0, 1, 0), yaw(-90.0f), pitch(0.0f),
                                speed(8.0f), sensitivity(0.1f) {
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    updateVectors();
}

// Apply gravity and handle vertical movement
void Camera::physics(float deltaTime) {
    // Accelerate downward due to gravity
    velocityY += GRAVITY * deltaTime;
    
    // Apply vertical movement
    position.y += velocityY * deltaTime;
    
    // Ground collision: stop at ground level
    if (position.y <= GROUND_Y) {
        position.y = GROUND_Y;
        velocityY = 0.0f;
        isOnGround = true;
    } else {
        isOnGround = false;
    }
}

// Get the view matrix for rendering
glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

// Recalculate camera direction vectors based on yaw and pitch angles
void Camera::updateVectors() {
    // Calculate front vector from yaw and pitch angles
    front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ));
    
    // Calculate right vector perpendicular to front
    right = glm::normalize(glm::cross(front, worldUp));
    
    // Recalculate up vector perpendicular to front and right
    up = glm::normalize(glm::cross(right, front));
}

// Handle keyboard input for movement
void Camera::processKeyboard(int direction, float deltaTime) {
    // Calculate movement speed for this frame
    float velocity = speed * deltaTime * 1.5;
    
    // Store current Y position to prevent physics interference
    float currentY = position.y;
    
    // Apply horizontal movement based on direction
    if (direction == 0) position += front * velocity;      // Forward
    if (direction == 1) position -= front * velocity;      // Backward
    if (direction == 2) position -= right * velocity;      // Left
    if (direction == 3) position += right * velocity;      // Right
    
    // Restore Y position so only physics affects vertical movement
    position.y = currentY;
    
    // Apply world boundary constraints to keep player in world
    float limit = 19.0f;
    if (position.x < -limit) position.x = -limit;
    if (position.x > limit) position.x = limit;
    if (position.z < -limit) position.z = -limit;
    if (position.z > limit) position.z = limit;
}

// Jump if player is on ground
void Camera::jump() {
    if (isOnGround) {
        velocityY = JUMP_FORCE;  // Set upward velocity
        isOnGround = false;
        std::cout << "Jump!\n";
    }
}

// Handle mouse movement for looking around
void Camera::processMouse(float xoffset, float yoffset) {
    // Update rotation angles based on mouse movement
    yaw += xoffset * sensitivity;
    pitch += yoffset * sensitivity;
    
    // Clamp pitch to prevent flipping view
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    
    // Recalculate direction vectors
    updateVectors();

}
