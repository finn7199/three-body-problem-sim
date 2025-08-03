#pragma once

#include <glm/glm.hpp>
#include <vector>

// Window Settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Simulation Settings
const float G = 500.0f;          // Gravitational Constant (tweaked for visual appeal)
const int TRAIL_LENGTH = 250;   // Number of points in the trail

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
        100.0f, 5.0f,                          // mass, radius
        glm::vec3(-100.0f, 0.0f, 0.0f),      // position
        glm::vec3(15.19f, 22.84f, 0.0f),     // velocity
        glm::vec3(1.0f, 0.5f, 0.0f)         // color (Orange)
    },
    {
        100.0f, 5.0f,
        glm::vec3(100.0f, 0.0f, 0.0f),
        glm::vec3(15.19f, 22.84f, 0.0f),
        glm::vec3(0.0f, 0.6f, 1.0f)         // color (Blue)
    },
    {
        100.0f, 5.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(-30.38f, -45.68f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f)         // color (White)
    }
};