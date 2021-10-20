#version 430 core
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out int material;

in vec3 vertex_pos;
in vec3 vertex_normal;
in vec2 vertex_tex;

layout(location = 0) uniform sampler2D texture0; // Color
//layout(location = 2) uniform sampler2D texture2; // Normal (TODO?)

void main()
{
    color = texture(texture0, vertex_tex).rgb;
    normal = vertex_normal;
    position = vertex_pos;
    material = 7; // Container material ID
}