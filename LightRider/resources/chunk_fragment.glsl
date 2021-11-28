#define GLOW_AMOUNT 10

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

const vec4 player1Accent = vec4(0.1, 0.5, 1, 1);
const vec4 player2Accent = vec4(1, 0.5, 0.1, 1);

uniform int playerId;
uniform sampler2D texture0;

void main()
{
	if (texture(texture0, vertex_tex).r > 0.5)
	{
		vec4 accent = playerId == 0
			? player1Accent
			: player2Accent;

		writeOutput(accent * GLOW_AMOUNT, vertex_pos, vertex_normal, 0);
	}
	else
	{
		writeOutput(vec4(0, 0, 0, 1), vertex_pos, vertex_normal, 0);
	}
}
