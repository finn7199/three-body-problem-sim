#include "Body.h"
#include "simulation_config.h" 
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

Body::Body(float m, float r, glm::vec3 pos, glm::vec3 vel, glm::vec3 col)
    : mass(m), radius(r), position(pos), velocity(vel), color(col), acceleration(0.0f) {
    setupSphereMesh();
}

void Body::setupSphereMesh() {
    glGenVertexArrays(1, &bodyVAO);
    glGenBuffers(1, &bodyVBO);
    glGenBuffers(1, &bodyEBO);

    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<unsigned int> indices;

    const unsigned int X_SEGMENTS = 32;
    const unsigned int Y_SEGMENTS = 32;
    const float PI = 3.14159265359f;

    for (unsigned int y = 0; y <= Y_SEGMENTS; ++y) {
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
            float xSegment = (float)x / (float)X_SEGMENTS;
            float ySegment = (float)y / (float)Y_SEGMENTS;
            float xPos = radius * std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
            float yPos = radius * std::cos(ySegment * PI);
            float zPos = radius * std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

            positions.push_back(glm::vec3(xPos, yPos, zPos));
            normals.push_back(glm::normalize(glm::vec3(xPos, yPos, zPos)));
        }
    }

    bool oddRow = false;
    for (unsigned int y = 0; y < Y_SEGMENTS; ++y) {
        if (!oddRow) {
            for (unsigned int x = 0; x <= X_SEGMENTS; ++x) {
                indices.push_back(y * (X_SEGMENTS + 1) + x);
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
            }
        }
        else {
            for (int x = X_SEGMENTS; x >= 0; --x) {
                indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                indices.push_back(y * (X_SEGMENTS + 1) + x);
            }
        }
        oddRow = !oddRow;
    }
    indexCount = indices.size();

    std::vector<float> data;
    for (size_t i = 0; i < positions.size(); ++i) {
        data.push_back(positions[i].x);
        data.push_back(positions[i].y);
        data.push_back(positions[i].z);
        if (normals.size() > 0) {
            data.push_back(normals[i].x);
            data.push_back(normals[i].y);
            data.push_back(normals[i].z);
        }
    }

    glBindVertexArray(bodyVAO);
    glBindBuffer(GL_ARRAY_BUFFER, bodyVBO);
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bodyEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    GLsizei stride = 6 * sizeof(float);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
}


void Body::draw(Shader& sphereShader) {
    // --- Draw the Body ---
    sphereShader.use();
    sphereShader.setVec3("objectColor", color);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    sphereShader.setMat4("model", model);

    glBindVertexArray(bodyVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
}