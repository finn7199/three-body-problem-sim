#pragma once

#include <vector>
#include "Body.h"
#include "Shader.h"

class Simulation {
public:
    std::vector<Body> bodies;

    Simulation();
    void update(float dt);
    void draw(Shader& sphereShader);

private:
    // Helper function to calculate accelerations for a given state
    std::vector<glm::vec3> calculateAccelerations(const std::vector<Body>& currentBodies);
};