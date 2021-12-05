#version 430 core
layout(location = 0) out vec4 _color;
layout(location = 1) out vec3 _position;
layout(location = 2) out vec3 _normal;
layout(location = 3) out int _material;

#define VOXEL_CAMERA_SIZE 20.
#define VOXEL_CAMERA_SIZE_INV (1. / VOXEL_CAMERA_SIZE)
#define VOXEL_MAP_DIMENSION 128
#define VOXEL_MAP_HALF_DIMENSION (VOXEL_MAP_DIMENSION / 2)

uniform int _outputMode;
uniform vec3 _voxelCenterPosition;

layout(r32i, binding = 1) uniform coherent iimage3D _voxelMapR;
layout(r32i, binding = 2) uniform coherent iimage3D _voxelMapG;
layout(r32i, binding = 3) uniform coherent iimage3D _voxelMapB;
layout(r32i, binding = 4) uniform coherent iimage3D _voxelMapA;

ivec3 worldToVoxelCoordinates(vec3 pos)
{
    pos -= _voxelCenterPosition;
    pos *= VOXEL_CAMERA_SIZE_INV;
    pos *= VOXEL_MAP_DIMENSION;
    pos += vec3(VOXEL_MAP_HALF_DIMENSION);

    return ivec3(pos);
}

void writeOutput(vec4 col, vec3 pos, vec3 norm, int mat)
{
    if (_outputMode == 0)
    {
        _color = col;
        _position = pos;
        _normal = norm;
        _material = mat;
    }
    else if (_outputMode == 1)
    {
        ivec3 voxelPos = worldToVoxelCoordinates(pos);
        if (voxelPos.x < 0 || voxelPos.y < 0 || voxelPos.z < 0 ||
            voxelPos.x >= VOXEL_MAP_DIMENSION || voxelPos.y >= VOXEL_MAP_DIMENSION || voxelPos.z >= VOXEL_MAP_DIMENSION)
        {
            return;
        }

        imageAtomicAdd(_voxelMapR, voxelPos, int(255 * col.r)); // TODO: Multiply color amount by alpha?
        imageAtomicAdd(_voxelMapG, voxelPos, int(255 * col.g));
        imageAtomicAdd(_voxelMapB, voxelPos, int(255 * col.b));
        imageAtomicAdd(_voxelMapA, voxelPos, 1);

        // For debugging.
        _color = col;
    }
    else
    {
        // No-op: Invalid output mode.
    }
}
