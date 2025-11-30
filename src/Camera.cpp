#include "Camera.h"
#include <glm/gtc/constants.hpp>
#include <iostream>

Camera::Camera(glm::vec3 pos) : position(pos), worldUp(0,1,0), yaw(-90.0f), pitch(0.0f),
                                speed(8.0f), sensitivity(0.1f) {
    front = glm::vec3(0.0f, 0.0f, -1.0f);
    updateVectors();
}

void Camera::physics(float deltaTime) {
    // Apply gravity
    velocityY += GRAVITY * deltaTime;
    
    // Apply Y movement
    position.y += velocityY * deltaTime;
    
    // Ground collision
    if (position.y <= GROUND_Y) {
        position.y = GROUND_Y;
        velocityY = 0.0f;
        isOnGround = true;
    } else {
        isOnGround = false;
    }
}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::updateVectors() {
    front = glm::normalize(glm::vec3(
        cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
        sin(glm::radians(pitch)),
        sin(glm::radians(yaw)) * cos(glm::radians(pitch))
    ));
    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

void Camera::processKeyboard(int direction, float deltaTime) {
    float velocity = speed * deltaTime;
    
    // Store current Y position before movement
    float currentY = position.y;
    
    // Apply horizontal movement
    if (direction == 0) position += front * velocity;
    if (direction == 1) position -= front * velocity;
    if (direction == 2) position -= right * velocity;
    if (direction == 3) position += right * velocity;
    
    // Restore Y position - let physics handle vertical movement
    position.y = currentY;
    
    // Apply world boundary constraints
    float limit = 19.0f;
    if (position.x < -limit) position.x = -limit;
    if (position.x >  limit) position.x =  limit;
    if (position.z < -limit) position.z = -limit;
    if (position.z >  limit) position.z =  limit;
}

void Camera::jump() {
    if (isOnGround) {
        velocityY = JUMP_FORCE;
        isOnGround = false;
        std::cout << "Jump!\n";
    }
}

void Camera::processMouse(float xoffset, float yoffset) {
    yaw   += xoffset * sensitivity;
    pitch += yoffset * sensitivity;
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;
    updateVectors();
}