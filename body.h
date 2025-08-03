#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <vector>
#include "Shader.h"

class Body {
public:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    float mass;
    glm::vec3 color;
    float radius;

    Body(float m, float r, glm::vec3 pos, glm::vec3 vel, glm::vec3 col);
    void draw(Shader& sphereShader);

private:
    unsigned int bodyVAO, bodyVBO, bodyEBO;
    unsigned int indexCount;

    void setupSphereMesh();
};