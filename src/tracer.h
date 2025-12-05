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
    
    // Add a tracer bullet effect from start to end position
    void add(const glm::vec3& start, const glm::vec3& end);
    
    // Update all tracers (reduce lifetime)
    void update(float deltaTime);
    
    // Render all active tracers with projection and view matrices
    void render(unsigned int shaderID, const glm::mat4& projection, const glm::mat4& view) const;
};