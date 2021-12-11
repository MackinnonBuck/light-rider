#version 430 core

layout(location = 0) out vec4 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out int material;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D texture0;

uniform float bloomFactor;

void main()
{
    position = vertex_pos;
    normal = vertex_normal;
    material = 8;

    color = texture(texture0, vertex_tex);
    color.rgb *= pow((color.r + color.g + color.b), bloomFactor);
}
