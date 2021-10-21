#version 430 core

#define KERNEL_SIZE 64
#define RADIUS 0.5
#define BIAS 0.025

const vec3 player1Color = vec3(0.0, 0.75, 1.0);
const vec3 player2Color = vec3(1.0f, 0.25f, 0.0f);

in vec2 texCoords;

out vec3 fragColor;

layout(location = 0) uniform sampler2D gColor;
layout(location = 1) uniform sampler2D gPosition;
layout(location = 2) uniform sampler2D gNormal;
layout(location = 3) uniform isampler2D gMaterial;
layout(location = 4) uniform sampler2D noise;

uniform vec3 campos;
uniform vec3 samples[KERNEL_SIZE];
uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec2 noiseScale = vec2(textureSize(gPosition, 0).xy) * 0.25;
    fragColor = texture(gColor, texCoords).xyz;

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

        occlusion += (sampleDepth >= samplePos.z + BIAS ? 1.0 : 0.0);
    }

    occlusion = 1.0 - (occlusion / KERNEL_SIZE);
    fragColor.rgb *= vec3(occlusion, occlusion, occlusion);
}
