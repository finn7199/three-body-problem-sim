#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

in vec3 FragPos;
in vec3 Normal;

//UNIFORMS
uniform vec3 objectColor;
uniform bool isEmissive;

// Multi-light uniforms
const int MAX_LIGHTS = 3;
uniform vec3 lightPositions[MAX_LIGHTS];
uniform vec3 lightColors[MAX_LIGHTS];
uniform bool lightEmissive[MAX_LIGHTS];
uniform int numLights;
uniform int bodyIndex;

uniform vec3 viewPos;
uniform float metalness;
uniform float roughness;
uniform samplerCube irradianceMap;

const float PI = 3.14159265359;

// GGX/Trowbridge-Reitz normal distribution function
float DistributionGGX(vec3 N, vec3 H, float rough)
{
    float a  = rough * rough;
    float a2 = a * a;
    float NdotH  = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;

    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return a2 / max(denom, 0.0000001);
}

// Schlick-Beckmann geometry function (single direction)
float GeometrySchlickGGX(float NdotV, float rough)
{
    float r = (rough + 1.0);
    float k = (r * r) / 8.0;

    float denom = NdotV * (1.0 - k) + k;

    return NdotV / max(denom, 0.0000001);
}

// Smith's method combining both view and light directions
float GeometrySmith(vec3 N, vec3 V, vec3 L, float rough)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2  = GeometrySchlickGGX(NdotV, rough);
    float ggx1  = GeometrySchlickGGX(NdotL, rough);

    return ggx1 * ggx2;
}

// Fresnel-Schlick approximation
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Roughness-aware Fresnel for ambient energy conservation
vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float rough)
{
    return F0 + (max(vec3(1.0 - rough), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

// Cook-Torrance BRDF for a single point light
vec3 CookTorranceDirect(vec3 albedo, vec3 N, vec3 V, vec3 lightPos, vec3 lightColor, vec3 F0, float rough, float metal)
{
    vec3 L = normalize(lightPos - FragPos);
    vec3 H = normalize(V + L);
    float NdotL = max(dot(N, L), 0.0);

    // Cook-Torrance specular BRDF
    float NDF = DistributionGGX(N, H, rough);
    float G   = GeometrySmith(N, V, L, rough);
    vec3  F   = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator    = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * NdotL;
    vec3 specular     = numerator / max(denominator, 0.0000001);

    // Energy conservation: diffuse component
    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metal);

    // Lambertian diffuse (divided by PI)
    vec3 diffuse = kD * albedo / PI;

    return (diffuse + specular) * lightColor * NdotL;
}

void main()
{
    vec3 finalColor;
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // Clamp roughness to avoid GGX singularity at 0
    float rough = clamp(roughness, 0.05, 1.0);

    // Metalness workflow: dielectrics ~4% reflectance, metals use albedo
    vec3 F0 = mix(vec3(0.04), objectColor, metalness);

    if (isEmissive)
    {
        // Self-emission glow
        vec3 emission = objectColor * 2.0;

        // Accumulate lighting from OTHER emissive bodies
        vec3 lighting = vec3(0.0);
        for (int i = 0; i < numLights; i++) {
            if (i == bodyIndex) continue;   // skip self
            if (!lightEmissive[i]) continue; // only lit by other emissive bodies

            lighting += CookTorranceDirect(objectColor, norm, viewDir, lightPositions[i], lightColors[i], F0, rough, metalness);
        }

        finalColor = emission + lighting;
    }
    else
    {
        // Ambient from IBL (diffuse only, no specular IBL)
        vec3 F_ambient = fresnelSchlickRoughness(max(dot(norm, viewDir), 0.0), F0, rough);
        vec3 kD_ambient = (1.0 - F_ambient) * (1.0 - metalness);
        vec3 irradiance = texture(irradianceMap, norm).rgb;
        vec3 ambient = kD_ambient * irradiance * objectColor;

        // Accumulate direct lighting from all emissive bodies
        vec3 directLighting = vec3(0.0);
        for (int i = 0; i < numLights; i++) {
            if (!lightEmissive[i]) continue;

            directLighting += CookTorranceDirect(objectColor, norm, viewDir, lightPositions[i], lightColors[i], F0, rough, metalness);
        }

        finalColor = ambient + directLighting;
    }

    FragColor = vec4(finalColor, 1.0);

    // Bright pass for bloom
    float brightness = dot(finalColor, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > 1.0)
        BrightColor = vec4(finalColor, 1.0);
    else
        BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
}
