#version 410 core
out vec4 color;
in vec2 vertex_tex;

uniform sampler2D screenTexture;

void main()
{             
    color.rgb = texture(screenTexture, vertex_tex).rgb;
    color.a = 1.0;

    float brightness = (color.r + color.g + color.b) * 0.33333;

    if (brightness < 1.0)
    {
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    else
    {
        color.rgb = normalize(color.rgb);
        color.rgb += color.rgb * (brightness - 1);
    }
}
