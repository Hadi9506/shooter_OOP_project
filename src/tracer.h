// Tracer.h
#pragma once
#include <glm/glm.hpp>
#include <vector>

struct Tracer {
    glm::vec3 start;
    glm::vec3 end;
    float life = 0.2f;
};

class TracerManager {
public:
    std::vector<Tracer> tracers;
    void add(const glm::vec3& start, const glm::vec3& end);
    void update(float deltaTime);
    void render(unsigned int shaderID, const glm::mat4& projection, const glm::mat4& view)  const;  // ← pass shader + VP
};