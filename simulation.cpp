#include "Simulation.h"
#include "simulation_config.h"

Simulation::Simulation() {
    // Standard initialization. RK4 does not require a special starting step.
    for (const auto& props : initialBodies) {
        bodies.emplace_back(props.mass, props.radius, props.initialPosition, props.initialVelocity, props.color);
    }
}

// Private helper function to calculate all accelerations
std::vector<glm::vec3> Simulation::calculateAccelerations(const std::vector<Body>& currentBodies) {
    std::vector<glm::vec3> accelerations;
    accelerations.reserve(currentBodies.size());

    for (const auto& body : currentBodies) {
        glm::vec3 total_acceleration(0.0f);
        for (const auto& other : currentBodies) {
            if (&body == &other) continue;
            float dist_sq = glm::length2(other.position - body.position);
            if (dist_sq < 0.1f) dist_sq = 0.1f;
            glm::vec3 force_dir = glm::normalize(other.position - body.position);
            glm::vec3 force = force_dir * (G * body.mass * other.mass / dist_sq);
            total_acceleration += force / body.mass;
        }
        accelerations.push_back(total_acceleration);
    }
    return accelerations;
}


// ---=== RUNGE-KUTTA 4 (RK4) IMPLEMENTATION ===---
void Simulation::update(float dt) {
    const int numBodies = bodies.size();

    // Store original state
    std::vector<Body> originalBodies = bodies;

    // Create arrays for the k-values for each body
    std::vector<glm::vec3> k1_pos(numBodies), k1_vel(numBodies);
    std::vector<glm::vec3> k2_pos(numBodies), k2_vel(numBodies);
    std::vector<glm::vec3> k3_pos(numBodies), k3_vel(numBodies);
    std::vector<glm::vec3> k4_pos(numBodies), k4_vel(numBodies);

    // --- Step 1: Calculate k1 ---
    std::vector<glm::vec3> initial_accel = calculateAccelerations(originalBodies);
    for (int i = 0; i < numBodies; ++i) {
        k1_pos[i] = originalBodies[i].velocity;
        k1_vel[i] = initial_accel[i];
    }

    // --- Step 2: Calculate k2 ---
    std::vector<Body> tempBodies_k2 = originalBodies;
    for (int i = 0; i < numBodies; ++i) {
        tempBodies_k2[i].position += k1_pos[i] * (dt / 2.0f);
    }
    std::vector<glm::vec3> k2_accel = calculateAccelerations(tempBodies_k2);
    for (int i = 0; i < numBodies; ++i) {
        k2_pos[i] = originalBodies[i].velocity + k1_vel[i] * (dt / 2.0f);
        k2_vel[i] = k2_accel[i];
    }

    // --- Step 3: Calculate k3 ---
    std::vector<Body> tempBodies_k3 = originalBodies;
    for (int i = 0; i < numBodies; ++i) {
        tempBodies_k3[i].position += k2_pos[i] * (dt / 2.0f);
    }
    std::vector<glm::vec3> k3_accel = calculateAccelerations(tempBodies_k3);
    for (int i = 0; i < numBodies; ++i) {
        k3_pos[i] = originalBodies[i].velocity + k2_vel[i] * (dt / 2.0f);
        k3_vel[i] = k3_accel[i];
    }

    // --- Step 4: Calculate k4 ---
    std::vector<Body> tempBodies_k4 = originalBodies;
    for (int i = 0; i < numBodies; ++i) {
        tempBodies_k4[i].position += k3_pos[i] * dt;
    }
    std::vector<glm::vec3> k4_accel = calculateAccelerations(tempBodies_k4);
    for (int i = 0; i < numBodies; ++i) {
        k4_pos[i] = originalBodies[i].velocity + k3_vel[i] * dt;
        k4_vel[i] = k4_accel[i];
    }

    // --- Step 5: Update final position and velocity ---
    for (int i = 0; i < numBodies; ++i) {
        bodies[i].position += (dt / 6.0f) * (k1_pos[i] + 2.0f * k2_pos[i] + 2.0f * k3_pos[i] + k4_pos[i]);
        bodies[i].velocity += (dt / 6.0f) * (k1_vel[i] + 2.0f * k2_vel[i] + 2.0f * k3_vel[i] + k4_vel[i]);
    }
}


void Simulation::draw(Shader& sphereShader) {
    for (auto& body : bodies) {
        body.draw(sphereShader);
    }
}