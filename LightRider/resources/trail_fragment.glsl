#version 430 core
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out float material;

uniform int playerId;
uniform float noiseSeed;
uniform float currentTime;

in vec3 fragmentPosition;
in float fragmentHeight;
in float fragmentTimeStamp;

void main()
{
    // TODO: We should really consider making the material data a vec4 and moving this data
    // into that.
    color.rgb = vec3(fragmentHeight, fragmentTimeStamp, noiseSeed);
    normal.r = currentTime;
    position = fragmentPosition;
    material = 4 + playerId; // Use 4 or 5 depending on which bike is emitting the trail.
}
