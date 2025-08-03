#pragma once

#include <glm/glm.hpp>
#include <vector>

// Window Settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Simulation Settings
const float G = 1500.0f; 
const float FIXED_TIMESTEP = 1.0f / 1000.0f;  // Run physics at x Hz

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
        10000.0f, 25.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 0.7f)
    },
    {
        // "Planet" - Now more massive and further away
        1000.0f, 20.0f,                       // Increased mass from 300 to 1000
        glm::vec3(400.0f, 0.0f, 0.0f),      // Increased distance from 250 to 400
        glm::vec3(0.0f, 195.0f, 0.0f),      // Adjusted orbital velocity for the new distance
        glm::vec3(0.4f, 0.6f, 1.0f)
    },
    {
        // "Moon"
        30.0f, 4.0f,                      // Increased mass from 10 to 30, and radius from 3 to 4
        glm::vec3(420.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 195.0f + 60.0f, 0.0f),
        glm::vec3(0.8f, 0.8f, 0.8f)
    }
};