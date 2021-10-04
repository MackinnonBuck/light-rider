#version 430 core
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out int material;

#define ARROW_SPEED 0.5
#define ARROW_WIDTH 0.1
#define ARROW_SPACING 0.5

uniform float time;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec3 vertex_pos_raw;

void main()
{
    if (mod(vertex_pos_raw.x + abs(vertex_pos_raw.z) - time * ARROW_SPEED, ARROW_SPACING) < ARROW_WIDTH)
    {
        color = vec3(1, 1, 1);
        material = 6;
    }
    else
    {
        color = vec3(0.0, 0.25, 1) * 0.1;
        material = 4;
    }

    position = vertex_pos;
    normal = vertex_normal;
}
