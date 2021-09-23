#version 430 core
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out float material;

in vec3 vertex_normal;
in vec3 vertex_pos;

uniform vec3 bikeColor;

uniform sampler2D texture0;

void main()
{
    color.rgb = texture(texture0, vec2(vertex_pos.x, vertex_pos.z) * 0.05).rgb;
    material = 1.0f;
}
