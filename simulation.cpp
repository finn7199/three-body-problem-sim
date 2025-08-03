#include "Simulation.h"
#include "simulation_config.h"

Simulation::Simulation() {
    for (const auto& props : initialBodies) {
        bodies.emplace_back(props.mass, props.radius, props.initialPosition, props.initialVelocity, props.color);
    }
}

// --- EULER METHOD IMPLEMENTATION ---
void Simulation::update(float dt) {
    // Step 1: Calculate forces and accelerations for all bodies
    for (auto& body : bodies) {
        body.acceleration = glm::vec3(0.0f);
        for (auto& other : bodies) {
            if (&body == &other) continue;

            float dist_sq = glm::length2(other.position - body.position);
            // Add a small epsilon to prevent division by zero if bodies overlap
            if (dist_sq < 0.1f) dist_sq = 0.1f;

            glm::vec3 force_dir = glm::normalize(other.position - body.position);
            glm::vec3 force = force_dir * (G * body.mass * other.mass / dist_sq);
            body.acceleration += force / body.mass;
        }
    }

    // Step 2: Update velocities and positions based on the calculated accelerations
    for (auto& body : bodies) {
        body.velocity += body.acceleration * dt;
        body.position += body.velocity * dt;

        // Update trail
        body.trail.push_back(body.position);
        if (body.trail.size() > TRAIL_LENGTH) {
            body.trail.pop_front();
        }
    }
}

void Simulation::draw(Shader& sphereShader, Shader& trailShader) {
    for (auto& body : bodies) {
        body.draw(sphereShader, trailShader);
    }
}