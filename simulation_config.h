#pragma once

#include <glm/glm.hpp>
#include <vector>

// Window Settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Simulation Settings
const float G = 1500.0f; 
const float FIXED_TIMESTEP = 1.0f / 120.0f; // Run physics at 120Hz

// Define the properties for each body
struct BodyProperties {
    float mass;
    float radius;
    glm::vec3 initialPosition;
    glm::vec3 initialVelocity;
    glm::vec3 color;
};

// An array holding the properties of our three bodies.
const std::vector<BodyProperties> initialBodies = {
    {
        // "Star" - Unchanged
        10000.0f, 15.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.7f)
    },
    {
        // "Planet" - Now more massive and further away
        1000.0f, 10.0f,                       // Increased mass from 300 to 1000
        glm::vec3(400.0f, 0.0f, 0.0f),      // Increased distance from 250 to 400
        glm::vec3(0.0f, 195.0f, 0.0f),      // Adjusted orbital velocity for the new distance
        glm::vec3(0.4f, 0.6f, 1.0f)
    },
    {
        // "Moon" - Now in a tighter orbit around the more massive planet
        10.0f, 3.0f,
        glm::vec3(420.0f, 0.0f, 0.0f),      // Positioned closer to the planet (20 units away)
        // Velocity is the Planet's velocity + a smaller orbital velocity around the planet
        glm::vec3(0.0f, 195.0f + 60.0f, 0.0f),
        glm::vec3(0.8f, 0.8f, 0.8f)
    }
};