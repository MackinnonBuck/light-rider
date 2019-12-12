#version 330 core
out vec4 color;

in vec3 vertex_normal;
in vec3 vertex_pos;

uniform vec3 bikeColor;

uniform sampler2D texture0;

void main()
{
    color = texture(texture0, vec2(vertex_pos.x, vertex_pos.z) * 0.05);
    float lightness = (color.r + color.g + color.b) / 3;
    lightness = pow(lightness, 4);
    lightness *= 0.5f;
    color.rgb = vec3(lightness);
    color.a = 0.75;
}
