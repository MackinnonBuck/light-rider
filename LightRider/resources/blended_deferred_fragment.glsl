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
layout(location = 5) uniform sampler2D indirectTexture;

uniform float occlusionFactor;

void main()
{
    vec2 texelSize = 1.0 / vec2(textureSize(ssaoTexture, 0));
    
    // Blurred version
    float occlusion = 0;
    vec4 indirectLight = vec4(0.);
    for (int x = -KERNEL_HALF_DIM; x < KERNEL_HALF_DIM; x++)
    {
        for (int y = -KERNEL_HALF_DIM; y < KERNEL_HALF_DIM; y++)
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            occlusion += texture(ssaoTexture, texCoords + offset).r;
            indirectLight += texture(indirectTexture, texCoords + offset);
        }
    }
    occlusion /= KERNEL_SIZE;
    indirectLight /= KERNEL_SIZE;

    // Non-blurred version
    //float occlusion = texture(ssaoTexture, texCoords).r;

    fragColor = texture(gColor, texCoords).rgb;
    float occlusionWeight = 1;
//    occlusionWeight *= occlusionFactor;
    fragColor.rgb = (1 - occlusionWeight) * fragColor.rgb + occlusionWeight * occlusion * fragColor.rgb;
    fragColor.rgb += indirectLight.rgb * occlusion;

    // TODO: Additional post processing effects here.
}
