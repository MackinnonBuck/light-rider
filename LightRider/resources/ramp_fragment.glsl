#define ARROW_SPEED 0.5
#define ARROW_WIDTH 0.1
#define ARROW_SPACING 0.5

uniform float time;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec3 vertex_pos_raw;

void main()
{
    if (mod(vertex_pos_raw.x + abs(vertex_pos_raw.z) - time * ARROW_SPEED, ARROW_SPACING) < ARROW_WIDTH)
    {
        writeOutput(vec4(1, 1, 1, 1), vertex_pos, vertex_normal, 6);
    }
    else
    {
        writeOutput(vec4(vec3(0.0, 0.25, 1) * 0.1, 1), vertex_pos, vertex_normal, 4);
    }
}
