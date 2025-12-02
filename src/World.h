#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Cube {
    glm::vec3 pos;
    glm::vec3 size;
    glm::vec3 color;
    bool isItem;          // false = wall, true = pick-up item
    int   id;             // unique for items
};

class World {
public:
    std::vector<Cube> cubes;
    void generate();
    void regenerateItems();  // ✅ NEW: Respawn items
    void render(unsigned int VAO, const glm::mat4& VP, unsigned int shaderID) const;
};