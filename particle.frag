#version 330 core
out vec4 FragColor;

in vec4 vColor;
in float vLife;

void main()
{
    // Make the particle a soft round point 
    if (length(gl_PointCoord - vec2(0.5)) > 0.5) {
        discard;
    }

    // The particle's alpha fades with its life
    FragColor = vec4(vColor.rgb, vColor.a * vLife);
}