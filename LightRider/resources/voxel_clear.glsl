#version 450 
layout(local_size_x = 8, local_size_y = 4, local_size_z = 4) in;	

layout(r32f, binding = 1) uniform image3D voxelMapR;
layout(r32f, binding = 2) uniform image3D voxelMapG;
layout(r32f, binding = 3) uniform image3D voxelMapB;
layout(r32f, binding = 4) uniform image3D voxelMapA;

void main()
{
    ivec3 tc = ivec3(gl_GlobalInvocationID.xyz);
    imageStore(voxelMapR, tc, vec4(0));
    imageStore(voxelMapG, tc, vec4(0));
    imageStore(voxelMapB, tc, vec4(0));
    imageStore(voxelMapA, tc, vec4(0));
}
