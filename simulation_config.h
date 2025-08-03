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
    // mass: Determines the body's gravitational pull.
    // radius: The visual size of the sphere in 3D space.
    // initial position (x, y, z): Where the body starts.
    // initial velocity (vx, vy, vz): The direction and speed it starts with.
    // color (R, G, B): The color of the sphere, from 0.0 to 1.0.

    {
        // P1
        5000.0f, 25.0f,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, -60.0f),
        glm::vec3(1.0f, 1.0f, 0.7f)
    },
    {
        // P2
        5000.0f, 20.0f,                       
        glm::vec3(420.0f, -50.0f, 0.0f),      
        glm::vec3(0.0f, -60.0f, 0.0f), 
        glm::vec3(0.4f, 0.6f, 1.0f)
    },
    {
        // P3
        5000.0f, 13.0f,
        glm::vec3(-420.0f, 50.0f, 0.0f), 
        glm::vec3(0.0f, 60.0f, 0.0f),
        glm::vec3(0.8f, 0.8f, 0.8f)
    }
};