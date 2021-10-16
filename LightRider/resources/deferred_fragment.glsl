#version 430 core

const vec3 player1Color = vec3(0.0, 0.75, 1.0);
const vec3 player2Color = vec3(1.0f, 0.25f, 0.0f);

in vec2 texCoords;

out vec3 fragColor;

layout(location = 0) uniform sampler2D gColor;
layout(location = 1) uniform sampler2D gPosition;
layout(location = 2) uniform sampler2D gNormal;
layout(location = 3) uniform isampler2D gMaterial;
layout(location = 4) uniform sampler2D shadowMap;

uniform vec3 campos;
uniform mat4 lightPV;

// Evaluates how shadowed a point is using PCF with 5 samples
// Credit: Sam Freed - https://github.com/sfreed141/vct/blob/master/shaders/phong.frag
float calcShadowFactor(vec4 lightSpacePosition)
{
    vec3 shifted = (lightSpacePosition.xyz / lightSpacePosition.w + 1.0) * 0.5;

    float shadowFactor = 0;
    float bias = 0.00001;
    float fragDepth = shifted.z - bias;

    if (fragDepth > 1.0)
    {
        return 0.0;
    }

    const int numSamples = 5;
    const ivec2 offsets[numSamples] = ivec2[](
        ivec2(0, 0), ivec2(1, 0), ivec2(0, 1), ivec2(-1, 0), ivec2(0, -1)
    );

    for (int i = 0; i < numSamples; i++)
    {
        if (fragDepth > textureOffset(shadowMap, shifted.xy, offsets[i]).r)
        {
            shadowFactor += 1;
        }
    }
    shadowFactor /= numSamples;

    return shadowFactor;
}

vec3 lightPositions[4] = vec3[]
(
    vec3(1000.0, 500.0, 1000.0),
    vec3(-1000.0, 500.0, 1000.0),
    vec3(1000.0, 500.0, -1000.0),
    vec3(-1000.0, 500.0, -1000.0)
);

vec3 sunDirection = normalize(vec3(1.0f, -0.5f, -1.0f));

vec3 getDiffuseFromLight(vec3 fragPosition, vec3 normal, vec3 lightDir)
{
    vec3 lightColor = vec3(1.0);
    float light = dot(lightDir, normal);	
    return clamp(light, 0.0f, 1.0f) * lightColor;
}

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

void main()
{
    //float depth = texture(shadowMap, texCoords).r;
    //fragColor = vec3(1 - depth);

    //return;

    int materialId = texture(gMaterial, texCoords).r;

    // Simple materials
    if (materialId == 0)
    {
        // No material; in color = out color.
        fragColor = texture(gColor, texCoords).rgb;
        return;
    }

    // Materials utilizing fragment color:
    fragColor = texture(gColor, texCoords).rgb;

    if (materialId == 1)
    {
        // Player 1 bike accent
        fragColor.rgb *= player1Color * 6;
        return;
    }

    if (materialId == 2)
    {
        // Player 2 bike accent
        fragColor.rgb *= player2Color * 6;
        return;
    }

    if (materialId == 3)
    {
        // Bike headlights
        fragColor.rgb = vec3(10);
        return;
    }

    if (materialId == 6)
    {
        // Ramp arrows
        fragColor.rgb = vec3(0.5, 1, 0.8) * 6;
        return;
    }

    // Complex materials with lighting and shadows:
    vec3 fragPosition = texture(gPosition, texCoords).rgb;
    vec3 fragNormal = texture(gNormal, texCoords).rgb;
    vec3 n = normalize(-fragNormal);
    vec4 lightSpacePosition = lightPV * vec4(fragPosition, 1.0);
    float shadowFactor = calcShadowFactor(lightSpacePosition);
    float lightFactor = 1 - shadowFactor;

    if (materialId == 4)
    {
        // Bike body
        for (int i = 0; i < 4; i++)
            fragColor.rgb += vec3(getSpecFromLight(fragPosition, n, lightPositions[i], 500) * 8) * lightFactor;

        fragColor.rgb *= getDiffuseFromLight(fragPosition, n, sunDirection);

        return;
    }

    if (materialId == 5)
    {
        // Bike rider
        fragColor.rgb = vec3(0);

        for (int i = 0; i < 4; i++)
            fragColor.rgb += vec3(getSpecFromLight(fragPosition, n, lightPositions[i], 80) * 0.5) * lightFactor;

        return;
    }
    
    // Default - render magenta because this should never happen.
    fragColor = vec3(1, 0, 1);
}
