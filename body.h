#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include "Shader.h"
#include "ParticleSystem.h"

class Body {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    glm::vec3 color;
    float radius;
    bool isEmissive;
    float metalness;
    float roughness;

    Body(float m, float r, glm::vec3 pos, glm::vec3 vel, glm::vec3 col, bool emissive, float met, float rough);
    void draw(Shader& sphereShader) const;

    // Public members for the particle trails
    std::unique_ptr<ParticleSystem> Trail;
    void UpdateTrail(float dt);
    void RenderTrail(const glm::mat4& view, const glm::mat4& projection) const;

private:
    unsigned int bodyVAO, bodyVBO, bodyEBO;
    unsigned int indexCount;

    void setupSphereMesh();
};