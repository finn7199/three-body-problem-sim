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
    bool isEmissive;
};

// An array holding the properties of our three bodies.
const std::vector<BodyProperties> initialBodies = {
    // mass: Determines the body's gravitational pull.
    // radius: The visual size of the sphere in 3D space.
    // initial position (x, y, z): Where the body starts.
    // initial velocity (vx, vy, vz): The direction and speed it starts with.
    // color (R, G, B): The color of the sphere, from 0.0 to 1.0.
    // is Emmisive or not 

    {
        // "Sun"
        10000.0f, 20.0f,
        glm::vec3(0.0f),
        glm::vec3(0.0f),
        glm::vec3(1.0f, 0.9f, 0.4f), // yellow/orange color
        true
    },
    {
        // "Earth"
        300.0f, 15.0f,
        glm::vec3(350.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 205.0f, 0.0f),
        glm::vec3(0.3f, 0.5f, 1.0f),
        false
    },
    {
        // "Moon"
        10.0f, 5.0f,
        glm::vec3(360.0f, 0.0f, 0.0f),        // Positioned closer to Earth (10 units away)
        // Earth's velocity + a slower orbital velocity for the Moon
        glm::vec3(0.0f, 205.0f + 45.0f, 0.0f),
        glm::vec3(0.7f, 0.7f, 0.7f),
        false
    }
};