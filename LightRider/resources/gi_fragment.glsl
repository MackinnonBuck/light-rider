#version 430 core

// HBAO constants
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

// Voxel constants
#define VOXEL_CAMERA_SIZE 20.
#define VOXEL_CAMERA_SIZE_INV (1. / VOXEL_CAMERA_SIZE)
#define VOXEL_MAP_DIMENSION 128
#define VOXEL_MAP_HALF_DIMENSION (VOXEL_MAP_DIMENSION / 2)

in vec2 texCoords;

layout(location = 0) out float fragColor;
layout(location = 1) out vec4 indirectColor;

layout(location = 0) uniform sampler2D gPosition;
layout(location = 1) uniform sampler2D gNormal;
layout(location = 2) uniform isampler2D gMaterial;
layout(location = 3) uniform sampler2D noise;
layout(location = 4) uniform sampler3D voxelMap;

uniform mat4 view;
uniform vec2 focalLength;
uniform vec3 voxelCenterPosition;

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

float hbao(vec3 rand)
{
    vec2 aoResolution = textureSize(gPosition, 0);
    vec2 invAoResolution = 1.0 / aoResolution;

    // Position of fragment in "eye" space.
    vec3 p = fetchEyePos(texCoords, ivec2(0, 0));

//    // Calculate the random vector.
//    vec2 noiseScale = vec2(textureSize(gPosition, 0).xy) * 0.25;
//    vec3 rand = texture(noise, texCoords * noiseScale).xyz;
//
    vec2 rayRadiusUv = 0.5 * R * focalLength / -p.z;
    float rayRadiusPix = rayRadiusUv.x * aoResolution.x;

    if (rayRadiusPix < 1)
    {
        return 1.0;
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
    return ao;
}

vec3 worldToVoxelCoordinates(vec3 pos)
{
    pos -= voxelCenterPosition;
    pos *= VOXEL_CAMERA_SIZE_INV;
    pos += vec3(0.5);

    return pos;
}

vec3 traceCone(vec3 origin, vec3 dir, float angle)
{
    float voxelSize = float(VOXEL_CAMERA_SIZE) / float(VOXEL_MAP_DIMENSION);
    float t = voxelSize*2; // Starting ray dist (to prevent self-intersection and improve performance)
    vec4 color = vec4(0);
    for (int i = 0; i < 128; i++) {
        vec3 pos = origin + t*dir;
        float coneDiamater = t*2*tan(angle);
        float mipLevel = min(7., log2(1 + coneDiamater/voxelSize));
        vec3 voxelPos = worldToVoxelCoordinates(pos);

		if (voxelPos.x < 0 || voxelPos.y < 0 || voxelPos.z < 0 ||
			voxelPos.x > 1 || voxelPos.y > 1 || voxelPos.z > 1)
		{
			break;
		}

        vec4 sampleColor = textureLod(voxelMap, voxelPos, mipLevel);
        color += sampleColor * (1 - color.a) * log2(mipLevel + 1);
        t += voxelSize;
        if (color.a >= 0.95) {
            break;
        }
    } 

    return color.rgb * pow(1./max(t, 1.), 2);
}

vec4 voxelConeTracing(vec3 rand)
{
    vec3 normal = normalize(texture(gNormal, texCoords).rgb);
    vec3 position = texture(gPosition, texCoords).rgb;
    vec3 tangent = normalize(rand - normal * dot(rand, normal));
    vec3 bitangent = cross(normal, tangent);

    vec3 dirs[5] = {
        normal,
        normalize(tangent + normal),
        normalize(-tangent + normal),
        normalize(bitangent + normal),
        normalize(-bitangent + normal)
    };

    vec3 indirectLight = vec3(0.);

    for (int i = 0; i < 5; i++)
    {
        float diffusePdfWeight = dot(dirs[i], normal);
        indirectLight += diffusePdfWeight * traceCone(position, dirs[i], 22.5 * M_PI / 180.);
    }
    indirectLight /= 5;

    float actualDistance = length(position - voxelCenterPosition);
    float distanceFactor = max(0., 1. - (actualDistance * VOXEL_CAMERA_SIZE_INV * 2));

    indirectLight *= distanceFactor;

    return vec4(indirectLight, 1.);
}

void main()
{
    int material = texture(gMaterial, texCoords).r;

    // Calculate the random vector.
    vec2 noiseScale = vec2(textureSize(gPosition, 0).xy) * 0.25;
    vec3 rand = texture(noise, texCoords * noiseScale).xyz;

    // Don't use global illumination on the environment.
    if (material != 8)
    {
		indirectColor = voxelConeTracing(rand);

        switch (material)
        {
            case 9:
                indirectColor *= 0.25;
                break;
            default:
                break;
        }
    }
    else
    {
        indirectColor = vec4(0., 0., 0., 1.);
    }

    if (material == 7)
    {
		fragColor = hbao(rand);
    }
    else
    {
        fragColor = 1;
    }
}
