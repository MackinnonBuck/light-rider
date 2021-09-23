#version 430 core
layout(location = 0) out vec3 color;
layout(location = 3) out float material;

in vec3 fragmentPosition;
in vec3 fragmentColor;

void main()
{
    color = fragmentColor;
    material = 0.0f;
}
