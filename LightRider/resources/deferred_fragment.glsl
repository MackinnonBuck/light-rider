#version 430 core

in vec2 texCoords;

out vec3 fragColor;

layout(location = 0) uniform sampler2D gColor;
layout(location = 1) uniform sampler2D gPosition;
layout(location = 2) uniform sampler2D gNormal;
layout(location = 3) uniform isampler2D gMaterial;
layout(location = 4) uniform sampler2D shadowMap;
layout(location = 5) uniform sampler2D skyTexture;

uniform vec3 campos;
uniform mat4 lightPV;

// Prototypes for externally-defined functions.
bool computeSimpleMaterial(inout vec3 col, int mat);
bool computeComplexMaterial(inout vec3 col, vec3 pos, vec3 norm, vec3 campos, mat4 lightPV, sampler2D shadowMap, sampler2D skyTexture, int mat);

void main()
{
    int materialId = texture(gMaterial, texCoords).r;

    // Materials utilizing fragment color:
    fragColor = texture(gColor, texCoords).rgb;
    if (computeSimpleMaterial(fragColor, materialId))
    {
        return;
    }

    // Complex materials with lighting and shadows:
    vec3 fragPosition = texture(gPosition, texCoords).rgb;
    vec3 fragNormal = texture(gNormal, texCoords).rgb;
    if (computeComplexMaterial(fragColor, fragPosition, fragNormal, campos, lightPV, shadowMap, skyTexture, materialId))
    {
        return;
    }
    
    // Default - render magenta because this should never happen.
    fragColor = vec3(1, 0, 1);
}
