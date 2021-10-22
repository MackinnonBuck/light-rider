#version 430 core

#define KERNEL_DIM 4
#define KERNEL_HALF_DIM (KERNEL_DIM / 2)
#define KERNEL_SIZE (KERNEL_DIM * KERNEL_DIM)

in vec2 texCoords;

out vec3 fragColor;

layout(location = 0) uniform sampler2D gColor;
layout(location = 1) uniform sampler2D gPosition;
layout(location = 2) uniform sampler2D gNormal;
layout(location = 3) uniform isampler2D gMaterial;
layout(location = 4) uniform sampler2D ssaoTexture;

uniform float occlusionFactor;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    float occlusion = 0;
    for (int x = -KERNEL_HALF_DIM; x < KERNEL_HALF_DIM; x++)
    {
        for (int y = -KERNEL_HALF_DIM; y < KERNEL_HALF_DIM; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            occlusion += texture(ssaoTexture, texCoords + offset).r;
        }
    }
    occlusion /= KERNEL_SIZE;

    fragColor = texture(gColor, texCoords).rgb;
    vec3 occlusionColor = fragColor * occlusion;
    occlusionColor += vec3(0.1, 0.75, 1) * (1 - occlusion) * 3;
    fragColor = fragColor * (1 - occlusionFactor) + occlusionColor * occlusionFactor;

    // TODO: Additional post processing effects here.
}
