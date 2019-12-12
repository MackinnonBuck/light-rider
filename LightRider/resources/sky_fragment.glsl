#version 330 core
out vec4 color;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D texture3;

void main()
{
    color = texture(texture3, vertex_tex);
}
