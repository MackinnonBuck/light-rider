#version 410 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

noperspective out vec2 TexCoords;

void main()
{
	gl_Position = vec4(vertexPosition, 1.0);
	TexCoords = vertexTexture;
}
