#version 430 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexTexture;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 lightPV;

out vec3 vertex_pos;
out vec3 vertex_normal;
out vec4 vertex_light_space_pos;

void main()
{
	vertex_normal = vec4(M * vec4(vertexNormal ,0.0)).xyz;
	vec4 tpos =  M * vec4(vertexPosition, 1.0);
	vertex_pos = tpos.xyz;
	vertex_light_space_pos = lightPV * M * vec4(vertexPosition, 1.0);
	gl_Position = P * V * tpos;
}
