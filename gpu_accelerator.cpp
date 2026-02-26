#include "gpu_accelerator.h"
#include <iostream>

GpuAccelerator::GpuAccelerator()
    : m_shader(nullptr), m_positionSSBO(0), m_massSSBO(0),
      m_accelerationSSBO(0), m_maxBodies(0), m_initialized(false) {
}

GpuAccelerator::~GpuAccelerator() {
    if (m_initialized) {
        glDeleteBuffers(1, &m_positionSSBO);
        glDeleteBuffers(1, &m_massSSBO);
        glDeleteBuffers(1, &m_accelerationSSBO);
        delete m_shader;
    }
}

void GpuAccelerator::init(int maxBodies) {
    m_maxBodies = maxBodies;

    m_shader = new ComputeShader("gravity.comp");

    // Create SSBOs
    // Positions: vec4 per body (std430 alignment)
    glGenBuffers(1, &m_positionSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positionSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxBodies * sizeof(float) * 4, nullptr, GL_DYNAMIC_DRAW);

    // Masses: float per body
    glGenBuffers(1, &m_massSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_massSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxBodies * sizeof(float), nullptr, GL_DYNAMIC_DRAW);

    // Accelerations: vec4 per body (output)
    glGenBuffers(1, &m_accelerationSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_accelerationSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER, maxBodies * sizeof(float) * 4, nullptr, GL_DYNAMIC_READ);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    m_initialized = true;
}

void GpuAccelerator::uploadMasses(const std::vector<float>& masses) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_massSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, masses.size() * sizeof(float), masses.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

std::vector<glm::vec3> GpuAccelerator::calculateAccelerations(
    const std::vector<glm::vec3>& positions, float G, int numBodies)
{
    // Pack vec3 positions into vec4 array for GPU (std430 alignment)
    std::vector<float> posData(numBodies * 4);
    for (int i = 0; i < numBodies; ++i) {
        posData[i * 4 + 0] = positions[i].x;
        posData[i * 4 + 1] = positions[i].y;
        posData[i * 4 + 2] = positions[i].z;
        posData[i * 4 + 3] = 0.0f; // padding
    }

    // Upload positions
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_positionSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, numBodies * 4 * sizeof(float), posData.data());

    // Bind SSBOs to binding points
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_positionSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_massSSBO);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, m_accelerationSSBO);

    // Dispatch compute shader
    m_shader->use();
    m_shader->setFloat("u_G", G);
    m_shader->setInt("u_NumBodies", numBodies);

    unsigned int numGroups = (numBodies + 63) / 64;
    m_shader->dispatch(numGroups);

    // Read back accelerations
    std::vector<float> accelData(numBodies * 4);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_accelerationSSBO);
    glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, numBodies * 4 * sizeof(float), accelData.data());
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    // Unpack vec4 back to vec3
    std::vector<glm::vec3> accelerations(numBodies);
    for (int i = 0; i < numBodies; ++i) {
        accelerations[i] = glm::vec3(
            accelData[i * 4 + 0],
            accelData[i * 4 + 1],
            accelData[i * 4 + 2]
        );
    }

    return accelerations;
}
