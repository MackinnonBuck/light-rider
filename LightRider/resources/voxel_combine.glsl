#version 450 
layout(local_size_x = 8, local_size_y = 4, local_size_z = 4) in;	

layout(r32i, binding = 1) uniform iimage3D voxelMapR;
layout(r32i, binding = 2) uniform iimage3D voxelMapG;
layout(r32i, binding = 3) uniform iimage3D voxelMapB;
layout(r32i, binding = 4) uniform iimage3D voxelMapA;
layout(rgba16f, binding = 5) uniform image3D voxelMap;

void main()
{
    ivec3 tc = ivec3(gl_GlobalInvocationID.xyz);
    vec3 col = vec3(
        imageLoad(voxelMapR, tc).r / 255.,
        imageLoad(voxelMapG, tc).r / 255.,
        imageLoad(voxelMapB, tc).r / 255.);
    float count = imageLoad(voxelMapA, tc).r;
    imageStore(voxelMap, tc, vec4(col*min(1.f, count)/max(1.f, count), min(1.f, count)));
}
