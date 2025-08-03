#pragma once

#include <vector>
#include "Body.h"
#include "Shader.h"

class Simulation {
public:
    std::vector<Body> bodies;

    Simulation();
    void update(float dt);
    void draw(Shader& sphereShader);
};