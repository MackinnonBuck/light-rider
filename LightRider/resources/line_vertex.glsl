#version 430 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexColor;

uniform mat4 P;
uniform mat4 V;

out vec3 fragmentPosition;
out vec3 fragmentColor;

void main()
{
    fragmentPosition = vertexPosition;
    fragmentColor = vertexColor;

	gl_Position = P * V * vec4(vertexPosition, 1.0);
}
