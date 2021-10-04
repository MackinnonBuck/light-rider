#version 430 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in float vertexHeight;
layout(location = 2) in float vertexTimeStamp;

uniform mat4 P;
uniform mat4 V;

out vec3 fragmentPosition;
out float fragmentHeight;
out float fragmentTimeStamp;

void main()
{
    fragmentPosition = vertexPosition;
    fragmentHeight = vertexHeight;
    fragmentTimeStamp = vertexTimeStamp;

	gl_Position = P * V * vec4(vertexPosition, 1.0);
}
