#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float yaw;
    float pitch;
    float speed;
    float sensitivity;
    float velocityY = 0.0f;    
    bool isOnGround = true;
    const float GRAVITY = -15.0f;  
    const float JUMP_FORCE = 7.0f; 
    const float GROUND_Y = 2.0f;

    Camera(glm::vec3 pos = glm::vec3(0.0f, 0.0f, 3.0f));

    glm::mat4 getViewMatrix() const;
    void processKeyboard(int direction, float deltaTime,bool hasjumped); // 0=FWD,1=BACK,2=LEFT,3=RIGHT
    void processMouse(float xoffset, float yoffset);
    void physics(float deltaTime);
    void jump();
private:
    void updateVectors();

};