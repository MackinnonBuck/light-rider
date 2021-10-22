#version 430 core

#define SHADOW_STRENGTH 0.7
#define OPACITY 0.4

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out int material;

//out vec4 color;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec4 vertex_light_space_pos;

const float M_PI = 3.1415926535;

//uniform vec3 lightPosition;
//uniform vec3 lightDirection;

layout(location = 0) uniform sampler2D texture0;
layout(location = 3) uniform sampler2D texture3;
layout(location = 4) uniform sampler2D shadowMap;

uniform vec3 campos;

// Evaluates how shadowed a point is using PCF with 5 samples
// Credit: Sam Freed - https://github.com/sfreed141/vct/blob/master/shaders/phong.frag
float calcShadowFactor(vec4 lightSpacePosition)
{
    vec3 shifted = (lightSpacePosition.xyz / lightSpacePosition.w + 1.0) * 0.5;

    float shadowFactor = 0;
    float bias = 0.00001;
    float fragDepth = shifted.z - bias;

    if (fragDepth > 1.0)
    {
        return 0.0;
    }

    const int numSamples = 5;
    const ivec2 offsets[numSamples] = ivec2[](
        ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(-1, 0), ivec2(0, -1)
    );

    for (int i = 0; i < numSamples; i++)
    {
        if (fragDepth > textureOffset(shadowMap, shifted.xy, offsets[i]).r)
        {
            shadowFactor += 1;
        }
    }
    shadowFactor /= numSamples;

    return shadowFactor;
}

void main()
{
    position = vertex_pos;
    normal = vec3(0, 1, 0);
    material = 0;

    color = texture(texture0, vec2(vertex_pos.x, vertex_pos.z) * 0.05);
    float shadowFactor = calcShadowFactor(vertex_light_space_pos);

    float lightness = (color.r + color.g + color.b) / 3;
    lightness = pow(lightness, 4);
    lightness *= 0.5f;
    lightness += 0.15f;
    color.rgb = vec3(lightness);

    vec3 e = vertex_pos - campos;
    vec3 r = reflect(e, normal);

    if (r.y >= 0)
    {
        vec3 v = normalize(r);
        float x = (atan(v.z, v.x) + M_PI) / (M_PI * 2);
        float y = (v.y + 1) * 0.5;
        y = 1 - y;
        x = mod(x + 0.25, 1);
        color.rgb += texture(texture3, vec2(x, y)).rgb * 0.6;
    }

    color.rgb *= 1 - shadowFactor * SHADOW_STRENGTH;
    color.a = OPACITY + (shadowFactor * SHADOW_STRENGTH) * (1 - OPACITY);
}