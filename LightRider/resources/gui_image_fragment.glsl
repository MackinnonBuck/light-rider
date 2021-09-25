#version 330 core
out vec4 color;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform sampler2D texture0;

uniform float bloomFactor;

void main()
{
    color = texture(texture0, vertex_tex);
    color.rgb *= pow((color.r + color.g + color.b), bloomFactor);
}
