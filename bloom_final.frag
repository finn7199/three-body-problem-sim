#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D scene;
uniform sampler2D bloomBlur;
uniform float exposure;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(scene, TexCoords).rgb;
    vec3 bloomColor = texture(bloomBlur, TexCoords).rgb;
    
    hdrColor += bloomColor; // Additive bloom
    
    // Tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor * exposure);

    // Gamma correction
    result = pow(result, vec3(1.0 / gamma));

    // Use the alpha from the original scene render.
    // This makes the empty space transparent, letting the skybox show through.
    float alpha = texture(scene, TexCoords).a;
    
    FragColor = vec4(result, alpha);
}