#version 400 core
// outputs
out vec4 FragColor; // final fragment colour

// inputs
in vec3 Normal;     // surface normal
in vec3 WorldPos;   // world position coordinates
in vec2 TexCoords;  // texture coordiantes

// UNIFORMS (can be changed outside of shaders)
//==========
uniform vec3 viewPos;

// pbr porperties
uniform sampler2D metallicMap;
uniform sampler2D roughnessMap;
uniform sampler2D albedoMap;    // surface colour
uniform sampler2D normalMap;    // surface imperfections
uniform sampler2D aoMap;        // ambient occlusion


// lights
uniform vec3 lightPos[4];
uniform vec3 lightCol[4];

const float PI = 3.14159265359;

// function prototypes
vec3 FresnelFunc(float HdotV, vec3 F0);                        
float NormDistributionFunc(vec3 N, vec3 H, float roughness);
float GeometrySchlick(float NdotV, float roughness);
float GeometryFunc(vec3 N, vec3 V, float roughness);
vec3 getNormalMap();

void main()
{      
    // retrieve the material properties from the texture maps
    float metallic = texture(metallicMap, TexCoords).r;     
    float roughness = texture(roughnessMap, TexCoords).r;
    vec3 albedo = pow(texture(albedoMap, TexCoords).rgb, vec3(2.2));
    float ao = texture(aoMap, TexCoords).r;

    vec3 normal = normalize(Normal);
    vec3 viewDir = normalize(viewPos - WorldPos);

    vec3 F0 = vec3(0.04);           // set to a constant 0.04 for dielectrics
    F0 = mix(F0, albedo, metallic); // for metalic materials F0 is determined by the albedo and metalic properties

    vec3 Lo = vec3(0.0);                        // total reflected radiance
    for(int i = 0; i < 4; ++i) 
    {
        vec3 L = normalize(lightPos[i] - WorldPos);    // light direction
        vec3 H = normalize(viewDir + L);            // half way vector

        float dist = length(lightPos[i] - WorldPos);   // light ray distance
        float attentuaiton = 1.0 / (dist * dist);   // use ligth distance to calculate fall off
        vec3 radiance = lightCol[i] * attentuaiton;    // scale radiance based on attenuation

        // BRDF
        float NDF = NormDistributionFunc(normal, H, roughness);
        float G = GeometryFunc(normal, viewDir, roughness);
        vec3 F = FresnelFunc(max(dot(H, viewDir), 0.0), F0);

        vec3 kS = F;                // reflected light (specular)
        vec3 kD = vec3(1.0) - kS;   // refracted light (diffuse)
        kD *= 1.0 - metallic;       // metalic surfaces dont refract light    

        vec3 numerator = NDF * G * F;                           // calcualte DFG
        float denominator = 4.0 * max(dot(normal, viewDir), 0.0) * max(dot(normal, L), 0.0);
        vec3 specular = numerator / max(denominator, 0.001);    // work out the specular component using the BRDF

        // Reflectance Equation
        float NdotL = max(dot(normal, L), 0.0);
        Lo += (kD * albedo / PI + specular) * radiance * NdotL; // calcualate final reflectance value
    }
    
    vec3 ambient = vec3(0.03) * albedo * ao;    // make sure surfaces not in direct light are still lit
    vec3 colour = ambient + Lo;                  

    colour = colour / (colour + vec3(1.0));     // tone map HDR values to LDR
    colour = pow(colour, vec3(1.0 / 2.2));      // gamma correction

    FragColor = vec4(colour, 1.0);
}


// FUNCTIONS
//==========
// calculate specular to diffuse reflection ratio
vec3 FresnelFunc(float HdotV, vec3 F0)   
{
    return F0 + (1.0 - F0) * pow(max(1.0 - HdotV, 0.0), 5.0);
}

// Normal Distribution Function
float NormDistributionFunc(vec3 N, vec3 H, float roughness)
{
    float numerator = roughness*roughness;                  // work out (a) squared
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;                           // calculate (n dot h) squared

    float denominator = (NdotH2 * (numerator - 1.0) + 1.0); 
    denominator = PI * denominator * denominator;           // calculate the denominator

    return numerator / max(denominator, 0.0000001);         // return NDF
}

// Geometry Function
float GeometryFunc(vec3 N, vec3 V, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);  // calculate n dot v (return 0 if dot product is less than 0)

    float r = roughness + 1.0;
    float k = (r * r) / 8.0;            // calculate k for direct ligting

    float numerator = NdotV;
    float denominator = NdotV * (1.0 - k) + k;

    return numerator / denominator;
}