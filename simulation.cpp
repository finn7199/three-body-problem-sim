#include "Simulation.h"
#include "simulation_config.h"

Simulation::Simulation() {
    // Standard initialization. RK4 does not require a special starting step.
    for (const auto& props : initialBodies) {
        bodies.emplace_back(props.mass, props.radius, props.initialPosition, props.initialVelocity, props.color, props.isEmissive);
    }
}

// Function uses the passed-in positions and the class's 'bodies' member for mass.
std::vector<glm::vec3> Simulation::calculateAccelerations(const std::vector<glm::vec3>& positions) {
    std::vector<glm::vec3> accelerations(bodies.size(), glm::vec3(0.0f));

    for (int i = 0; i < bodies.size(); ++i) {
        for (int j = 0; j < bodies.size(); ++j) {
            if (i == j) continue;

            float dist_sq = glm::length2(positions[j] - positions[i]);
            if (dist_sq < 0.1f) dist_sq = 0.1f;

            glm::vec3 force_dir = glm::normalize(positions[j] - positions[i]);
            glm::vec3 force = force_dir * (G * bodies[i].mass * bodies[j].mass / dist_sq);
            accelerations[i] += force / bodies[i].mass;
        }
    }
    return accelerations;
}

void Simulation::update(float dt) {
    const int numBodies = bodies.size();

    // Store original state in simple vectors
    std::vector<glm::vec3> originalPositions(numBodies);
    std::vector<glm::vec3> originalVelocities(numBodies);
    for (int i = 0; i < numBodies; ++i) {
        originalPositions[i] = bodies[i].position;
        originalVelocities[i] = bodies[i].velocity;
    }

    std::vector<glm::vec3> k1_pos(numBodies), k1_vel(numBodies);
    std::vector<glm::vec3> k2_pos(numBodies), k2_vel(numBodies);
    std::vector<glm::vec3> k3_pos(numBodies), k3_vel(numBodies);
    std::vector<glm::vec3> k4_pos(numBodies), k4_vel(numBodies);

    // A temporary vector for positions only, which is safe to copy and modify.
    std::vector<glm::vec3> tempPositions(numBodies);

    // --- Step 1: Calculate k1 ---
    std::vector<glm::vec3> initial_accel = calculateAccelerations(originalPositions);
    for (int i = 0; i < numBodies; ++i) {
        k1_pos[i] = originalVelocities[i];
        k1_vel[i] = initial_accel[i];
    }

    // --- Step 2: Calculate k2 ---
    for (int i = 0; i < numBodies; ++i) {
        tempPositions[i] = originalPositions[i] + k1_pos[i] * (dt / 2.0f);
    }
    std::vector<glm::vec3> k2_accel = calculateAccelerations(tempPositions);
    for (int i = 0; i < numBodies; ++i) {
        k2_pos[i] = originalVelocities[i] + k1_vel[i] * (dt / 2.0f);
        k2_vel[i] = k2_accel[i];
    }

    // --- Step 3: Calculate k3 ---
    for (int i = 0; i < numBodies; ++i) {
        tempPositions[i] = originalPositions[i] + k2_pos[i] * (dt / 2.0f);
    }
    std::vector<glm::vec3> k3_accel = calculateAccelerations(tempPositions);
    for (int i = 0; i < numBodies; ++i) {
        k3_pos[i] = originalVelocities[i] + k2_vel[i] * (dt / 2.0f);
        k3_vel[i] = k3_accel[i];
    }

    // --- Step 4: Calculate k4 ---
    for (int i = 0; i < numBodies; ++i) {
        tempPositions[i] = originalPositions[i] + k3_pos[i] * dt;
    }
    std::vector<glm::vec3> k4_accel = calculateAccelerations(tempPositions);
    for (int i = 0; i < numBodies; ++i) {
        k4_pos[i] = originalVelocities[i] + k3_vel[i] * dt;
        k4_vel[i] = k4_accel[i];
    }

    // --- Step 5: Update final position and velocity in the actual 'bodies' vector ---
    for (int i = 0; i < numBodies; ++i) {
        bodies[i].position += (dt / 6.0f) * (k1_pos[i] + 2.0f * k2_pos[i] + 2.0f * k3_pos[i] + k4_pos[i]);
        bodies[i].velocity += (dt / 6.0f) * (k1_vel[i] + 2.0f * k2_vel[i] + 2.0f * k3_vel[i] + k4_vel[i]);

        ParticleProps props;

        glm::vec3 bodyVelocity = bodies[i].velocity;
        glm::vec3 emitPosition = bodies[i].position; // Default to center

        // Check oly offset if the body is moving
        if (glm::length2(bodyVelocity) > 0.001f) {
            // Get the direction opposite to the velocity
            glm::vec3 dirOppositeToVelocity = glm::normalize(-bodyVelocity);
            // Calculate the emission point on the "back" surface
            emitPosition = bodies[i].position + (dirOppositeToVelocity * bodies[i].radius);
        }
        props.Position = emitPosition;
        props.Position = bodies[i].position;
        props.ColorBegin = glm::vec4(bodies[i].color, 1.0);
        props.ColorEnd = glm::vec4(bodies[i].color, 0.0f);
        props.Velocity = bodies[i].velocity * -0.5f;
        props.VelocityVariation = glm::vec3(15.0f);
        props.LifeTime = 2.5f;
        bodies[i].Trail->Emit(props);
    }

    // Update all particle systems
    for (auto& body : bodies) {
        body.UpdateTrail(dt);
    }
}

void Simulation::draw(Shader& sphereShader) {
    for (auto& body : bodies) {
        body.draw(sphereShader);
    }
}