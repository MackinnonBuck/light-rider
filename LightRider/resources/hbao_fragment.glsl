#version 430 core

#define KERNEL_SIZE 64
#define M_PI 3.14159265
#define NUM_DIRECTIONS 8
#define NUM_STEPS 12
#define R 0.8
#define R2 (R*R)
#define NEG_INV_R2 -1.0 / R2
#define MAX_RADIUS_PIXELS 50.0
#define STRENGTH 1.9
#define TAN_ANGLE_BIAS tan(30.0 * M_PI / 180.0)

in vec2 texCoords;

out float fragColor;

layout(location = 0) uniform sampler2D gPosition;
layout(location = 1) uniform sampler2D gNormal;
layout(location = 2) uniform isampler2D gMaterial;
layout(location = 3) uniform sampler2D noise;

uniform mat4 view;
uniform vec2 focalLength;

float length2(vec3 v)
{
    return dot(v, v);
}

float invLength(vec2 v)
{
    return inversesqrt(dot(v, v));
}

float tangent(vec3 t)
{
    return -t.z * invLength(t.xy);
}

float tangent(vec3 p, vec3 s)
{
    return -(p.z - s.z) * invLength(s.xy - p.xy);
}

float biasedTangent(vec3 t)
{
    return t.z * invLength(t.xy) + TAN_ANGLE_BIAS;
}

vec3 minDiff(vec3 p, vec3 pr, vec3 pl)
{
    vec3 v1 = pr - p;
    vec3 v2 = p - pl;
    return (length2(v1) < length2(v2)) ? v1 : v2;
}

float falloff(float d2)
{
    return d2 * NEG_INV_R2 + 1.0;
}

vec2 snapUvOffset(vec2 uv, vec2 aoResolution, vec2 invAoResolution)
{
    return round(uv * aoResolution) * invAoResolution;
}

vec3 fetchEyePos(vec2 uv, ivec2 offset)
{
    return (view * textureOffset(gPosition, uv, offset)).xyz;
}

vec2 rotateDirections(vec2 dir, vec2 cosSin)
{
    return vec2(
        dir.x * cosSin.x - dir.y * cosSin.y,
        dir.x * cosSin.y + dir.y * cosSin.x);
}

void computeSteps(inout vec2 stepSizeUv, inout float numSteps, float rayRadiusPix, float rand, vec2 invAoResolution)
{
    numSteps = min(NUM_STEPS, rayRadiusPix);

    float stepSizePix = rayRadiusPix / (numSteps + 1);

    float maxNumSteps = MAX_RADIUS_PIXELS / stepSizePix;

    if (maxNumSteps < numSteps)
    {
        numSteps = floor(maxNumSteps + rand);
        numSteps = max(numSteps, 1);
        stepSizePix = MAX_RADIUS_PIXELS / numSteps;
    }

    stepSizeUv = stepSizePix * invAoResolution;
}

float horizonOcclusion(
    vec2 deltaUv,
    vec2 texelDeltaUv,
    vec2 uv0,
    vec3 p,
    float numSteps,
    float randStep,
    vec3 dPdu,
    vec3 dPdv,
    vec2 aoResolution,
    vec2 invAoResolution)
{
    float ao = 0;

    vec2 uv = uv0 + snapUvOffset(randStep * deltaUv, aoResolution, invAoResolution);
    deltaUv = snapUvOffset(deltaUv, aoResolution, invAoResolution);
    vec3 t = deltaUv.x * dPdu + deltaUv.y * dPdv;

    float tanH = biasedTangent(t);
    float sinH = tanH / sqrt(1.0 + tanH * tanH);

    for (float j = 1; j <= numSteps; j++)
    {
        uv += deltaUv;
        vec3 s = fetchEyePos(uv, ivec2(0, 0));
        float tanS = tangent(p, s);
        float d2 = length2(s - p);

        if ((d2 < R2) && (tanS > tanH))
        {
            float sinS = tanS / sqrt(1.0f + tanS * tanS);
            ao += falloff(d2) * (sinS - sinH);

            tanH = tanS;
            sinH = sinS;
        }
    }

    return ao;
}

void main()
{
    int material = texture(gMaterial, texCoords).r;

    if (material != 7)
    {
        fragColor = 1;
        return;
    }

    vec2 aoResolution = textureSize(gPosition, 0);
    vec2 invAoResolution = 1.0 / aoResolution;

    // Position of fragment in "eye" space.
    vec3 p = fetchEyePos(texCoords, ivec2(0, 0));

    // Calculate the random vector.
    vec2 noiseScale = vec2(textureSize(gPosition, 0).xy) * 0.25;
    vec3 rand = texture(noise, texCoords * noiseScale).xyz;

    vec2 rayRadiusUv = 0.5 * R * focalLength / -p.z;
    float rayRadiusPix = rayRadiusUv.x * aoResolution.x;

    if (rayRadiusPix < 1)
    {
        fragColor = 1.0;
        return;
    }

    float numSteps;
    vec2 stepSize;
    computeSteps(stepSize, numSteps, rayRadiusPix, rand.z, invAoResolution);

    vec3 pr, pl, pt, pb;
    pr = fetchEyePos(texCoords, ivec2(1, 0));
    pl = fetchEyePos(texCoords, ivec2(-1, 0));
    pt = fetchEyePos(texCoords, ivec2(0, 1));
    pb = fetchEyePos(texCoords, ivec2(0, -1));

    vec3 dPdu = minDiff(p, pr, pl);
    vec3 dPdv = minDiff(p, pt, pb) * (aoResolution.y * invAoResolution.x);

    float ao = 0;
    float d;
    float alpha = 2.0 * M_PI / NUM_DIRECTIONS;

    for (d = 0; d < NUM_DIRECTIONS; d++)
    {
        float angle = alpha * d;
        vec2 dir = rotateDirections(vec2(cos(angle), sin(angle)), rand.xy);
        vec2 deltaUv = dir * stepSize.xy;
        vec2 texelDeltaUv = dir * invAoResolution;
        ao += horizonOcclusion(deltaUv, texelDeltaUv, texCoords, p, numSteps, rand.z, dPdu, dPdv, aoResolution, invAoResolution);
    }

    ao = 1.0 - ao / NUM_DIRECTIONS * STRENGTH;
    fragColor = ao;
}
