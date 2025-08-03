#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    // Note there is no model matrix, trail vertices are already in world space
    gl_Position = projection * view * vec4(aPos, 1.0);
}