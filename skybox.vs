#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 TexCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    TexCoords = aPos;
    // Remove the translation part of the view matrix so the skybox follows the camera
    vec4 pos = projection * mat4(mat3(view)) * vec4(aPos, 1.0);
    // Set z to w to ensure the skybox is always at the far plane (behind everything else)
    gl_Position = pos.xyww;
}