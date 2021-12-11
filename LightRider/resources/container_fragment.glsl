in vec3 vertex_pos;
in vec3 vertex_normal;
in vec2 vertex_tex;

layout(location = 0) uniform sampler2D texture0; // Color
//layout(location = 2) uniform sampler2D texture2; // Normal (TODO?)

void main()
{
	writeOutput(texture(texture0, vertex_tex).rgba, vertex_pos, vertex_normal, 7);
}