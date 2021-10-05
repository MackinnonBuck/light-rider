#version 430 core
layout(location = 0) out vec3 color;
layout(location = 1) out vec3 position;
layout(location = 2) out vec3 normal;
layout(location = 3) out int material;

#define GLOW_AMOUNT 10

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

const vec3 player1Accent = vec3(0.1, 0.5, 1);
const vec3 player2Accent = vec3(1, 0.5, 0.1);

uniform int playerId;
uniform sampler2D texture0;

void main()
{
	material = 0;
	position = vertex_pos;
	normal = vertex_normal;

	if (texture(texture0, vertex_tex).r > 0.5)
	{
		vec3 accent = playerId == 0
			? player1Accent
			: player2Accent;

		color = accent * GLOW_AMOUNT;
	}
	else
	{
		color = vec3(0, 0, 0);
	}
}
