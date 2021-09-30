#version 430 core

const vec3 player1Color = vec3(0.0, 0.75, 1.0);
const vec3 player2Color = vec3(1.0f, 0.25f, 0.0f);

in vec2 texCoords;

out vec3 fragColor;

layout(location = 0) uniform sampler2D gColor;
layout(location = 1) uniform sampler2D gPosition;
layout(location = 2) uniform sampler2D gNormal;
layout(location = 3) uniform sampler2D gMaterial;
layout(location = 4) uniform sampler2D shadowMap;

uniform vec3 campos;

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

float getSpecFromLight(vec3 fragPosition, vec3 normal, vec3 lightPosition, float specularFactor)
{
    vec3 lightDirection = normalize(fragPosition - lightPosition);
    vec3 cameraDirection = normalize(fragPosition - campos);
    vec3 halfAngle = normalize(cameraDirection + lightDirection);
    float spec = dot(halfAngle, normal);
    spec = clamp(spec, 0, 1);
    spec = pow(spec, specularFactor);

    return spec;
}

void processBike(vec3 bikeColor)
{
    fragColor = texture(gColor, texCoords).rgb;
    vec3 fragPosition = texture(gPosition, texCoords).rgb;
    vec3 fragNormal = texture(gNormal, texCoords).rgb;

    vec3 n = normalize(-fragNormal);

    if (fragColor.r < 0.1) // Bike body
    {
        for (int i = 0; i < 4; i++)
            fragColor.rgb += vec3(getSpecFromLight(fragPosition, n, lightPositions[i], 500) * 8);
    }
    else if (fragColor.g < 0.1) // Bike rider
    {
        fragColor.rgb = vec3(0);

        for (int i = 0; i < 4; i++)
            fragColor.rgb += vec3(getSpecFromLight(fragPosition, n, lightPositions[i], 80) * 0.5);
    }
    else if (fragColor.b < 0.1) // Bike headlights
    {
        fragColor.rgb = vec3(20);
    }
    else // Bike accent
    {
        fragColor.rgb *= bikeColor * 6;
    }
}

void main()
{
    //float depth = texture(shadowMap, texCoords).r;
    //fragColor = vec3(1 - depth);

    //return;

	float materialId = texture(gMaterial, texCoords).r;

	if (materialId < 0.5) // ID 0
	{
		// No material; in color = out color.
		fragColor = texture(gColor, texCoords).rgb;
	}
	else if (materialId < 1.5) // ID 2
	{
		// Player 1 bike
        processBike(player1Color);
	}
    else if (materialId < 2.5) // ID 3
    {
        // Player 2 bike
        processBike(player2Color);
    }
	else
	{
		// No-op
		fragColor = vec3(0, 0, 0);
	}
}
