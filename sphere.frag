#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;

//UNIFORMS
uniform vec3 objectColor;
uniform bool isEmissive;

// Uniforms for lighting
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float shininess;
uniform samplerCube irradianceMap;

void main()
{		
    vec3 finalColor;

    if (isEmissive)
    {
        // If the body is a light source, just make it glow
        float emissionStrength = 4.0;
        finalColor = objectColor * emissionStrength;
    }
    else
    {
        // Otherwise, perform the full Blinn-Phong lighting calculation
        vec3 norm = normalize(Normal);
        
        // Ambient
        vec3 ambient = texture(irradianceMap, norm).rgb;

        // Diffuse
        vec3 lightDir = normalize(lightPos - FragPos);
        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * lightColor;

        // Specular
        float specularStrength = 0.4;
        vec3 viewDir = normalize(viewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);
        float spec = pow(max(dot(norm, halfwayDir), 0.0), shininess);
        vec3 specular = specularStrength * spec * lightColor;

        finalColor = (ambient + diffuse) * objectColor + specular;
    }
    
    FragColor = vec4(finalColor, 1.0);

    // Bright pass for bloom
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        BrightColor = vec4(finalColor, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}