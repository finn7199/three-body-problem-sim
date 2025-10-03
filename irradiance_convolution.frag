#version 330 core
out vec4 FragColor;
in vec3 WorldPos;

uniform samplerCube environmentMap;

void main()
{		
    vec3 N = normalize(WorldPos);
    vec3 blurredColor = vec3(0.0);

    // This is a simple, "brute-force" blur. It takes 9 samples from the
    // original skybox in a small area and averages them.
    float sampleOffset = 0.05; // How wide the blur is
    for (int x = -1; x <= 1; x++)
    {
        for (int y = -1; y <= 1; y++)
        {
            for (int z = -1; z <= 1; z++)
            {
                vec3 sampleDir = N + vec3(x, y, z) * sampleOffset;
                blurredColor += texture(environmentMap, normalize(sampleDir)).rgb;
            }
        }
    }
    blurredColor /= 27.0; // Average the 27 samples

    FragColor = vec4(blurredColor, 1.0);
}