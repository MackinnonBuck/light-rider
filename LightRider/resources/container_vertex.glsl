#version 430 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;
layout(location = 3) in vec4 instanceData;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec2 vertex_tex;

void main()
{
	vec3 instancePosition = instanceData.xyz;
	vec4 tpos =  M * vec4(vertexPosition * instanceData.w, 1.0) + vec4(instancePosition, 0.0);
	vertex_pos = tpos.xyz;
	vertex_tex = vec2(vertexTexture.x, -vertexTexture.y);
    vertex_normal = vec4(M * vec4(vertexNormal, 0.0)).xyz;
	gl_Position = P * V * tpos;
}
