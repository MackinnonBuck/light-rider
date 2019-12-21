#version 330 core
//out vec4 color;
layout(location = 0) out vec4 color;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNor;
layout(location = 3) out float fragReflect;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D texture3;

void main()
{
    color = texture(texture3, vertex_tex);
    fragPos = vertex_pos;
    fragNor = vertex_normal;
    fragReflect = 0.0f;
}
