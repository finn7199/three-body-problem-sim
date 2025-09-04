#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform bool isEmissive; // New uniform

void main()
{
    vec3 result;
    if (isEmissive) {
        // If emmisive, make it brighter
        result = objectColor * 1.25; 
    } else {
        // Otherwise, perform standard lighting for planets
        float ambientStrength = 0.1f;
        vec3 ambient = ambientStrength * lightColor;
        vec3 norm = normalize(Normal);
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;
        result = (ambient + diffuse) * objectColor;
    }
    
    FragColor = vec4(result, 1.0);

    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        BrightColor = vec4(result, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}