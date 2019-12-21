#version 400 core
layout(location = 0) out vec4 color;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNor;
layout(location = 3) out float fragReflect;

in vec3 vertex_normal;
in vec3 vertex_pos;

uniform sampler2D texture0;

void main()
{
    color = texture(texture0, vec2(vertex_pos.x, vertex_pos.z) * 0.05);

    float lightness = (color.r + color.g + color.b) / 3;
    lightness = pow(lightness, 4);
    lightness *= 0.5f;
    color.rgb = vec3(lightness);
    color.a = 0.75;

    fragPos = vertex_pos;
    fragNor = vec3(0.0f, 1.0f, 0.0f);//vertex_normal;
    fragReflect = 1.0f;
}
