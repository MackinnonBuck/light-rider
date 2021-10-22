#version 430 core

#define KERNEL_SIZE 64
#define RADIUS 0.5
#define BIAS 0.025

in vec2 texCoords;

out float fragColor;

layout(location = 0) uniform sampler2D gPosition;
layout(location = 1) uniform sampler2D gNormal;
layout(location = 2) uniform isampler2D gMaterial;
layout(location = 3) uniform sampler2D noise;

uniform vec3 samples[KERNEL_SIZE];
uniform mat4 projection;
uniform mat4 view;

void main()
{
    int material = texture(gMaterial, texCoords).r;

    if (material != 7)
    {
        fragColor = 1;
        return;
    }

    vec2 noiseScale = vec2(textureSize(gPosition, 0).xy) * 0.25;

    vec3 fragPos = (view * texture(gPosition, texCoords)).xyz;
    vec3 normal = normalize(mat3(view) * texture(gNormal, texCoords).xyz);
    vec3 randomVec = texture(noise, texCoords * noiseScale).xyz;

    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    float occlusion = 0.0;
    for (int i = 0; i < KERNEL_SIZE; i++)
    {
        vec3 samplePos = TBN * samples[i];
        samplePos = fragPos + samplePos * RADIUS;

        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + vec3(0.5, 0.5, 0.5);

        float sampleDepth = vec3(view * texture(gPosition, offset.xy)).z;
        float rangeCheck = smoothstep(0.0, 1.0, RADIUS / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0) * rangeCheck;
    }

    occlusion /= KERNEL_SIZE;
    occlusion = pow(occlusion, 0.7);
    occlusion = 1 - occlusion;
    fragColor = occlusion;
}
