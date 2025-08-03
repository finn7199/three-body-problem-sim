#include "Simulation.h"
#include "simulation_config.h"

Simulation::Simulation() {
    // Standard initialization from config file
    for (const auto& props : initialBodies) {
        bodies.emplace_back(props.mass, props.radius, props.initialPosition, props.initialVelocity, props.color);
    }

    // --- Leapfrog Initial Half-Step "Kick" ---
    // To start the Leapfrog method, we need the velocity at t(-1/2).
    // We calculate the initial acceleration at t(0) and use it to "kick" the velocity back by half a timestep.

    // First, calculate initial accelerations at t=0
    for (auto& body : bodies) {
        body.acceleration = glm::vec3(0.0f);
        for (auto& other : bodies) {
            if (&body == &other) continue;
            float dist_sq = glm::length2(other.position - body.position);
            if (dist_sq < 0.1f) dist_sq = 0.1f;
            glm::vec3 force_dir = glm::normalize(other.position - body.position);
            glm::vec3 force = force_dir * (G * body.mass * other.mass / dist_sq);
            body.acceleration += force / body.mass;
        }
    }

    // Now, kick the velocity back by half a step to get v(-dt/2)
    for (auto& body : bodies) {
        body.velocity -= body.acceleration * (FIXED_TIMESTEP / 2.0f);
    }
}


// ---=== LEAPFROG METHOD IMPLEMENTATION ===---
void Simulation::update(float dt) {
    // Step 1: "Drift" - Update positions by a full step using the half-step velocity
    for (auto& body : bodies) {
        body.position += body.velocity * dt;
    }

    // Step 2: Calculate new accelerations at the new positions
    for (auto& body : bodies) {
        body.acceleration = glm::vec3(0.0f);
        for (auto& other : bodies) {
            if (&body == &other) continue;
            float dist_sq = glm::length2(other.position - body.position);
            if (dist_sq < 0.1f) dist_sq = 0.1f;
            glm::vec3 force_dir = glm::normalize(other.position - body.position);
            glm::vec3 force = force_dir * (G * body.mass * other.mass / dist_sq);
            body.acceleration += force / body.mass;
        }
    }

    // Step 3: "Kick" - Update velocities to the next half-step using the new accelerations
    for (auto& body : bodies) {
        body.velocity += body.acceleration * dt;
    }
}

void Simulation::draw(Shader& sphereShader) {
    for (auto& body : bodies) {
        body.draw(sphereShader);
    }
}