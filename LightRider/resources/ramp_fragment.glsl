#version 430 core
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out float material;

in vec3 vertex_normal;
in vec3 vertex_pos;

void main()
{
    color = vec3(0, 0, 0);
    position = vertex_pos;
    normal = vertex_normal;
    material = 1.0f;
}
