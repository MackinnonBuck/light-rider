const vec3 _player1Color = vec3(0.0, 0.75, 1.0);
const vec3 _player2Color = vec3(1.0, 0.25, 0.0);

const float M_PI = 3.1415926535;

// Evaluates how shadowed a point is using PCF with 5 samples
// Credit: Sam Freed - https://github.com/sfreed141/vct/blob/master/shaders/phong.frag
float _calcShadowFactor(vec4 lightSpacePosition, sampler2D shadowMap)
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

vec3 _lightPositions[4] = vec3[]
(
    vec3(1000.0, 500.0, 1000.0),
    vec3(-1000.0, 500.0, 1000.0),
    vec3(1000.0, 500.0, -1000.0),
    vec3(-1000.0, 500.0, -1000.0)
);

vec3 _sunDirection = normalize(vec3(1.0f, -0.5f, -1.0f));

vec3 _getDiffuseFromLight(vec3 fragPosition, vec3 normal, vec3 lightDir)
{
    vec3 lightColor = vec3(1.0);
    float light = dot(lightDir, normal);
    return clamp(light, 0.0f, 1.0f) * lightColor;
}

float _getSpecFromLight(vec3 fragPosition, vec3 normal, vec3 campos, vec3 lightPosition, float specularFactor)
{
    vec3 lightDirection = normalize(fragPosition - lightPosition);
    vec3 cameraDirection = normalize(fragPosition - campos);
    vec3 halfAngle = normalize(cameraDirection + lightDirection);
    float spec = dot(halfAngle, normal);
    spec = clamp(spec, 0, 1);
    spec = pow(spec, specularFactor);

    return spec;
}

bool computeSimpleMaterial(inout vec3 col, int mat)
{
    switch (mat)
    {
        case 0:
            // No material; in color = out color.
            return true;
        case 1:
            // Player 1 bike accent
            col.rgb *= _player1Color * 6;
            return true;
        case 2:
            // Player 2 bike accent
            col.rgb *= _player2Color * 6;
            return true;
        case 3:
            // Bike headlights
            col.rgb = vec3(10);
            return true;
        case 6:
            // Ramp arrows
            col.rgb = vec3(1.0f, 1.0f, 1.0f) * 3;
            return true;
        default:
            return false;
    }
}

bool computeComplexMaterial(inout vec3 col, vec3 pos, vec3 norm, vec3 campos, mat4 lightPV, sampler2D shadowMap, sampler2D skyTexture, int mat)
{
    vec3 n = normalize(-norm);
    vec4 lightSpacePosition = lightPV * vec4(pos, 1.0);
    float shadowFactor = _calcShadowFactor(lightSpacePosition, shadowMap);
    float lightFactor = 1 - shadowFactor;

    switch (mat)
    {
        case 4:
        {
            // Bike body
            for (int i = 0; i < 4; i++)
                col.rgb += vec3(_getSpecFromLight(pos, n, campos, _lightPositions[i], 500) * 8) * lightFactor;

            col.rgb *= _getDiffuseFromLight(pos, n, _sunDirection);
            vec3 e = pos - campos;
            vec3 r = reflect(e, n);

            if (r.y >= 0)
            {
                vec3 v = normalize(r);
                float x = (atan(v.z, v.x) + M_PI) / (M_PI * 2);
                float y = (v.y + 1) * 0.5;
                y = 1 - y;
                x = mod(x + 0.25, 1);
                col.rgb += texture(skyTexture, vec2(x, y)).rgb * 0.8 * lightFactor;
            }

            return true;
        }
        case 5:
        {
            // Bike rider
            col.rgb = vec3(0);

            for (int i = 0; i < 4; i++)
                col.rgb += vec3(_getSpecFromLight(pos, n, campos, _lightPositions[i], 80) * 0.5) * lightFactor;

            return true;
        }
        case 7:
        {
            // Container
            vec3 specColor = vec3(0);

            for (int i = 0; i < 4; i++)
                specColor += vec3(_getSpecFromLight(pos, n, campos, _lightPositions[i], 20) * 2) * lightFactor;

            vec3 diffuseColor = _getDiffuseFromLight(pos, n, _sunDirection);
            col.rgb = (col.rgb + specColor) * diffuseColor + col.rgb * 0.1;
            return true;
        }
        default:
            return false;
    }
}
