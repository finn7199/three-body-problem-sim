#pragma once

#include <vector>
#include "Body.h"
#include "Shader.h"
#include "simulation_config.h"

class Simulation {
public:
    std::vector<Body> bodies;

    Simulation();
    void update(float dt);
    void draw(Shader& sphereShader);
    void LoadScenario(Scenario scenario);
    double CalculateTotalEnergy() const;
    void SetIntegrator(IntegratorMethod method);

private:
    float m_G;
    std::vector<glm::vec3> calculateAccelerations(const std::vector<glm::vec3>& positions);
    std::vector<glm::vec3> get_body_positions();

    IntegratorMethod m_currentMethod;
    void updateEuler(float dt);
    void updateLeapfrog(float dt);
    void updateRK4(float dt);
};