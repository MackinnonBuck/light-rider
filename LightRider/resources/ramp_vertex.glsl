#version 430 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

out vec3 vertex_pos;
out vec3 vertex_normal;

void main()
{
	vertex_normal = vec4(M * vec4(vertexNormal ,0.0)).xyz;
	vec4 tpos =  M * vec4(vertexPosition, 1.0);
	vertex_pos = tpos.xyz;
	gl_Position = P * V * tpos;
}
