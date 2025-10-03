#pragma once

#include <glm/glm.hpp>
#include <vector>
#include <map>

// Window Settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 900;

// Simulation Settings
const float G = 1000.0f;
const float FIXED_TIMESTEP = 1.0f / 1000.0f;  // Run physics at x Hz

// Scenario
enum class Scenario
{
    FIGURE_EIGHT,
    SOLAR_SYSTEM
};

// Integrator
enum class IntegratorMethod { EULER, LEAPFROG, RK4 };

// Define the properties for each body
// mass: Determines the body's gravitational pull.
// radius: The visual size of the sphere in 3D space.
// initial position (x, y, z): Where the body starts.
// initial velocity (vx, vy, vz): The direction and speed it starts with.
// color (R, G, B): The color of the sphere, from 0.0 to 1.0.
// is Emmisive or not 
// PBR metallic property (0=non-metal, 1=metal)
// PBR roughness property (0=smooth, 1=rough)
struct BodyProperties {
    float mass;
    float radius;
    glm::vec3 initialPosition;
    glm::vec3 initialVelocity;
    glm::vec3 color;
    bool isEmissive;
    float metalness;  
    float roughness;  
};

// Use a map to store all our scenario data
const std::map<Scenario, std::vector<BodyProperties>> scenarioData = {
    {
        Scenario::FIGURE_EIGHT, {
            { 1000.0f, 15.0f, glm::vec3(97.00133f, -24.30875f, 0.0f), glm::vec3(46.6203685f, 43.27659f, 0.0f), glm::vec3(1.0f, 0.2f, 0.1f), true, 0.1f, 0.2f },
            { 1000.0f, 15.0f, glm::vec3(-97.00133f, 24.30875f, 0.0f), glm::vec3(46.6203685f, 43.27659f, 0.0f), glm::vec3(0.1f, 0.5f, 1.0f), true, 0.1f, 0.2f },
            { 1000.0f, 15.0f, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-93.240737f, -86.55318f, 0.0f), glm::vec3(1.0f, 0.9f, 0.4f), true, 0.1f, 0.2f }
        }
    },
    {
        Scenario::SOLAR_SYSTEM, {
            { 5000.0f, 30.0f, glm::vec3(0.0f), glm::vec3(0.0f), glm::vec3(1.0f, 0.9f, 0.4f), true, 0.0f, 1.0f },
            { 20.0f, 15.0f, glm::vec3(400.0f, 0.0f, 0.0f), glm::vec3(0.0f, 135.0f, 0.0f), glm::vec3(0.3f, 0.5f, 1.0f), false, 0.1f, 0.4f },
            { 0.5f, 5.0f, glm::vec3(425.0f, 0.0f, 0.0f), glm::vec3(0.0f, 135.0f + 35.0f, 0.0f), glm::vec3(0.7f, 0.7f, 0.7f), false, 0.0f, 0.9f }
        }
    }
};