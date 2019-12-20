#version 410 core
out vec4 color;
in vec2 vertex_tex;

uniform sampler2D screenTexture;
uniform sampler2D screenPositions;
uniform sampler2D screenNormals;
uniform sampler2D reflectionMap;

void main()
{             
    color.rgb = texture(screenTexture, vertex_tex).rgb;
//    color.rgb = vec3(texture(reflectionMap, vertex_tex).r);
    color.a = 1.0;
}
