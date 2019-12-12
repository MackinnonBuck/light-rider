#version 410 core
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexTexture;

out vec2 vertex_tex;
out vec4 posPos;

uniform float screenWidth;
uniform float screenHeight;

float FXAA_SUBPIX_SHIFT = 1.0/4.0;

void main()
{
	gl_Position = vec4(vertexPosition, 1.0);
    vec2 rcpFrame = vec2(1.0 / screenWidth, 1.0 / screenHeight);
    vertex_tex = vertexTexture;
    posPos.xy = vertexTexture;
    posPos.zw = vertexTexture.xy - (rcpFrame * (0.5 + FXAA_SUBPIX_SHIFT));
}
