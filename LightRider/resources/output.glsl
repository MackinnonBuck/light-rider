#version 430 core
layout(location = 0) out vec4 _color;
layout(location = 1) out vec3 _position;
layout(location = 2) out vec3 _normal;
layout(location = 3) out int _material;

uniform int _outputMode;

layout(rgba16f, binding = 0) uniform image3D _voxelMap;

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
        // TODO
    }
    else
    {
        // No-op: Invalid output mode.
    }
}
