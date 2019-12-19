#version 400 core
layout(location = 0) out vec4 color;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec3 fragNor;

in vec3 vertex_normal;
in vec3 vertex_pos;
in vec2 vertex_tex;

uniform vec3 campos;
uniform vec3 bikeColor;

uniform sampler2D texture0;

// Bike colors:
// Body: 0, 0, 0
// Rider/Wheel: 1, 0, 0
// Headlights: 1, 1, 0
// Accent: 1, 1, 1

vec3 lightPositions[4] = vec3[]
(
    vec3(1000.0, 500.0, 1000.0),
    vec3(-1000.0, 500.0, 1000.0),
    vec3(1000.0, 500.0, -1000.0),
    vec3(-1000.0, 500.0, -1000.0)
);

float getSpecFromLight(vec3 normal, vec3 lightPosition, float specularFactor)
{
    vec3 lightDirection = normalize(vertex_pos - lightPosition);
    vec3 cameraDirection = normalize(vertex_pos - campos);
    vec3 halfAngle = normalize(cameraDirection + lightDirection);
    float spec = dot(halfAngle, normal);
    spec = clamp(spec, 0, 1);
    spec = pow(spec, specularFactor);

    return spec;
}

void main()
{
    fragPos = vertex_pos;
    fragNor = vertex_normal;

    color.a = 1;
    color.rgb = texture(texture0, vertex_tex).rgb;// * bikeColor;

    vec3 n = normalize(-vertex_normal);

    if (color.r < 0.1) // Bike body
    {
        for (int i = 0; i < 4; i++)
            color.rgb += vec3(getSpecFromLight(n, lightPositions[i], 500) * 8);
    }
    else if (color.g < 0.1) // Bike rider
    {
        color.rgb = vec3(0);

        for (int i = 0; i < 4; i++)
            color.rgb += vec3(getSpecFromLight(n, lightPositions[i], 80) * 0.5);
    }
    else if (color.b < 0.1) // Bike headlights
    {
        color.rgb = vec3(20);
    }
    else // Bike accent
    {
        color.rgb *= bikeColor * 6;
    }
}
