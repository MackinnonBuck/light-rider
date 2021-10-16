#version 450
#extension GL_ARB_shader_storage_buffer_object : require
#extension GL_NV_shader_atomic_float : require

layout(local_size_x = 1, local_size_y = 1) in;

layout(std430, binding=0) writeonly buffer result
{
    vec4 data[];
};

layout(rgba16f, binding = 0) uniform image2D colorImage;

float getLuminance(vec3 color)
{
    return 0.2126 * color.r + 0.7152 * color.g + 0.0722 * color.b;
}

void main()
{
    vec4 color = imageLoad(colorImage, ivec2(gl_GlobalInvocationID.xy));
    float luminance = getLuminance(color.rgb);
    atomicAdd(data[0].x, luminance);
}
