#version 410 core
out vec4 color;
in vec2 vertex_tex;

uniform sampler2D screenTexture;
uniform float exposure;

float getLuminance(vec3 color)
{
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void main()
{             
    color.rgb = texture(screenTexture, vertex_tex).rgb;
    color.a = 1.0;

    vec3 toneMapped = vec3(1.0) - exp(-color.rgb * exposure);
    float luminance = getLuminance(toneMapped);
    float threshold = 1 - exposure;
    if (luminance < threshold)
    {
        color.rgb = vec3(0.0, 0.0, 0.0);
    }
    else
    {
        color.rgb = color.rgb * ((luminance - threshold) / (1 - threshold));
    }
}
