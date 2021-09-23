#version 430 core
layout(location = 0) out vec3 color;
layout(location = 3) out float material;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D texture3;

void main()
{
    color = texture(texture3, vertex_tex).rgb;
    material = 0.0f;
}
