#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include <deque>
#include "Shader.h"

class Body {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    glm::vec3 color;
    float radius;

    std::deque<glm::vec3> trail;

    Body(float m, float r, glm::vec3 pos, glm::vec3 vel, glm::vec3 col);
    void draw(Shader& sphereShader, Shader& trailShader);

private:
    unsigned int bodyVAO, bodyVBO, bodyEBO;
    unsigned int trailVAO, trailVBO;
    unsigned int indexCount;

    void setupSphereMesh();
    void setupTrailMesh();
};