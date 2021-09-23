#version 330 core
layout(location = 0) out vec3 color;
layout(location = 3) out float material;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D texture0;

uniform float bloomFactor;

void main()
{
    color = texture(texture0, vertex_tex).rgb;
    color.rgb *= pow((color.r + color.g + color.b), bloomFactor);
    material = 0.0;
}
