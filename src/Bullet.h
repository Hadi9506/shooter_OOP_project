// Bullet.h
#pragma once
#include <glm/glm.hpp>

class Bullet {
public:
    virtual ~Bullet() = default;
    virtual float getDamage() const = 0;
    virtual glm::vec3 getTracerColor() const = 0;
    virtual int getMagSize() const = 0;
    virtual const char* getName() const = 0;
};

class SingleShotBullet : public Bullet {
public:
    float getDamage() const override { return 100.0f; }
    glm::vec3 getTracerColor() const override { return glm::vec3(1.0f, 0.2f, 0.2f); } // Red
    int getMagSize() const override { return 5; }
    const char* getName() const override { return "Single"; }
};

class ThreeHitBullet : public Bullet {
public:
    float getDamage() const override { return 40.0f; }
    glm::vec3 getTracerColor() const override { return glm::vec3(0.2f, 1.0f, 0.2f); } // Green
    int getMagSize() const override { return 10; }
    const char* getName() const override { return "Triple"; }
};

class FiveHitBullet : public Bullet {
public:
    float getDamage() const override { return 20.0f; }
    glm::vec3 getTracerColor() const override { return glm::vec3(1.0f, 1.0f, 0.3f); } // Yellow
    int getMagSize() const override { return 15; }
    const char* getName() const override { return "Five"; }
};
