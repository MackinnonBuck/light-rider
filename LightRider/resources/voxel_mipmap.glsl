#version 450 
layout(local_size_x = 8, local_size_y = 4, local_size_z = 4) in;	

layout(rgba16f, binding = 0) uniform image3D inMip;
layout(rgba16f, binding = 1) uniform image3D outMip;

void main()
{
    ivec3 tc = ivec3(gl_GlobalInvocationID.xyz);
    vec4 c = vec4(0.);
    c += imageLoad(inMip, 2 * tc + ivec3(0, 0, 0));
    c += imageLoad(inMip, 2 * tc + ivec3(0, 0, 1));
    c += imageLoad(inMip, 2 * tc + ivec3(0, 1, 0));
    c += imageLoad(inMip, 2 * tc + ivec3(0, 1, 1));
    c += imageLoad(inMip, 2 * tc + ivec3(1, 0, 0));
    c += imageLoad(inMip, 2 * tc + ivec3(1, 0, 1));
    c += imageLoad(inMip, 2 * tc + ivec3(1, 1, 0));
    c += imageLoad(inMip, 2 * tc + ivec3(1, 1, 1));
    c /= 8.;
    imageStore(outMip, tc, c);
}
