#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

uniform vec3 albedo;

void main()
{
    float emissionStrength = 8.0f;
    vec3 finalColor = albedo * emissionStrength;

    FragColor = vec4(finalColor, 1.0);
    BrightColor = vec4(finalColor, 1.0); // Always bright for bloom
}