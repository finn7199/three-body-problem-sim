#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>
#include "compute_shader.h"

class GpuAccelerator {
public:
    GpuAccelerator();
    ~GpuAccelerator();

    void init(int maxBodies);
    void uploadMasses(const std::vector<float>& masses);
    std::vector<glm::vec3> calculateAccelerations(const std::vector<glm::vec3>& positions, float G, int numBodies);

private:
    ComputeShader* m_shader;
    unsigned int m_positionSSBO;
    unsigned int m_massSSBO;
    unsigned int m_accelerationSSBO;
    int m_maxBodies;
    bool m_initialized;
};
