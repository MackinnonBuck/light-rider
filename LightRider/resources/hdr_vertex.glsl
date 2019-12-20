#version 410 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

out vec2 vertex_tex;

void main()
{
	gl_Position = vec4(vertexPosition, 1.0);
	vertex_tex = vertexTexture;
}
