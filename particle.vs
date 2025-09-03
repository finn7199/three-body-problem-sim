#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aLife;

out vec4 vColor;
out float vLife;

uniform mat4 projection;
uniform mat4 view;
uniform float u_SizeMultiplier;

void main()
{
    vColor = aColor;
    vLife = aLife;
    
    gl_Position = projection * view * vec4(aPos, 1.0);
    
    // A smaller multiplier makes particles smaller.
    gl_PointSize = u_SizeMultiplier * aLife;
}