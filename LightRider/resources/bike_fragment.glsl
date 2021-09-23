#version 430 core
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out float material;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform int playerId;

uniform sampler2D texture0;

void main()
{
    color.rgb = texture(texture0, vertex_tex).rgb;
    position = vertex_pos;
    normal = vertex_normal;
    material = 2 + float(playerId); // 2 or 3 depending on orange or blue bike.
}
