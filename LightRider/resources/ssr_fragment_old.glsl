#version 410 core
out vec4 color;
in vec2 vertex_tex;

//uniform vec3 viewPos;
uniform mat4 Pcam;
uniform mat4 Vcam;

uniform sampler2D screenTexture;
uniform sampler2D screenPositions;
uniform sampler2D screenNormals;
uniform sampler2D reflectionMap;

const float stepwidth = 0.1;
const float minRayStep = 0.1;
const float maxSteps = 30;
const int numBinarySearchSteps = 5;
const float reflectionSpecularFalloffExponent = 3.0;
#define Scale vec3(.8, .8, .8)
#define K 19.19
const int NUM_SAMPLES = 100;

vec3 BinarySearch(inout vec3 dir, inout vec3 hitCoord, inout float dDepth)
{
    float depth;

    vec4 projectedCoord;
 
    for(int i = 0; i < numBinarySearchSteps; i++)
    {
        projectedCoord = Pcam * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
        vec2 coords =  projectedCoord.xy;
        coords.y=1.-coords.y;
        coords.x=1.-coords.x;
        //depth = textureLod(viewpos, coords.xy, 2).z;
//        depth = length(viewPos - texture(screenPositions, coords).xyz);

//        float depth = (texture(screenPositions, vertex_tex) * Vcam).z;
//        vec3 camPos = Vcam[3].xyz;
//        depth = length(camPos - texture(screenPositions, coords).xyz);
// 

        depth = textureLod(screenPositions, projectedCoord.xy, 2.0).z;
        dDepth = hitCoord.z - depth;

        dir *= 0.5;
        if(dDepth > 0.0)
            hitCoord += dir;
        else
            hitCoord -= dir;    
    }

    projectedCoord = Pcam * vec4(hitCoord, 1.0);
    projectedCoord.xy /= projectedCoord.w;
    projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
 
    return vec3(projectedCoord.xy, depth);
}
//----------------------------------------------------------------------------------
vec4 RayMarch(vec3 dir, inout vec3 hitCoord, out float dDepth)
{
    float depth;
    int steps;
    vec4 projectedCoord;

    dir *= 10.55; //some factor, change for other, better results

    for(int i = 0; i < maxSteps; i++)
    {
        hitCoord += dir;
 
        projectedCoord = Pcam * vec4(hitCoord, 1.0);
        projectedCoord.xy /= projectedCoord.w;
        projectedCoord.xy = projectedCoord.xy * 0.5 + 0.5;
        vec2 coords =  projectedCoord.xy;
        coords.y=1.-coords.y;
        coords.x=1.-coords.x;
//        depth = textureLod(viewpos, coords.xy, 2).z;
//        vec3 camPos = Vcam[3].xyz;
//        depth = length(camPos - texture(screenPositions, coords).xyz);
//

//        float depth = (texture(screenPositions, vertex_tex) * Vcam).z;
        depth = textureLod(screenPositions, projectedCoord.xy, 2.0).z;

        if(depth > 1000.0)
            continue;

        dDepth = hitCoord.z - depth;

        if((dir.z - dDepth) < 1.2)
        {
            if(dDepth <= 0.0)
            {  
                vec4 Result;
                // projectedCoord.x = 1-projectedCoord.x;
                //return vec4(projectedCoord.xy, dDepth, 1.0);
                float ddd;
                Result = vec4(BinarySearch(dir, hitCoord, dDepth), ddd);

                return Result;
            }
        }
       
        steps++;
    }
 
    return vec4(projectedCoord.xy, depth, 0.0);
}

vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}


vec3 hash(vec3 a)
{
    a = fract(a * Scale);
    a += dot(a, a.yxz + K);
    return fract((a.xxy + a.yxx)*a.zyx);
}

void main()
{             
    float reflecton = texture(reflectionMap, vertex_tex).r;
    vec3 viewPos = textureLod(screenPositions, vertex_tex, 2).xyz;

    if(reflecton>0.1)
    {
        vec3 viewNormal = vec3(inverse(Vcam)*vec4(texture(screenNormals, vertex_tex).xyz,0));
        vec3 albedo = texture(screenTexture, vertex_tex).rgb;
        float spec  = 1;//= texture(ColorBuffer, TexCoords).w;
        float Metallic = 1;
        vec3 F0 = vec3(0.04);
        F0 = mix(F0, albedo, Metallic);
        vec3 Fresnel = fresnelSchlick(max(dot(normalize(viewNormal), normalize(viewPos)), 0.0), F0);

        // Reflection vector
        vec3 reflected = normalize(reflect(normalize(viewPos), normalize(viewNormal)));
        vec3 hitPos = viewPos;
        float dDepth;   
//        vec3 jitt = mix(vec3(0.0), vec3(hash(worldPos.xyz)), spec);
        vec3 jitt = mix(vec3(0.0), vec3(hash(texture(screenPositions, vertex_tex).xyz)), spec);
        vec4 coords = RayMarch((reflected * max(minRayStep, -viewPos.z)), hitPos, dDepth);
        coords.y=1.-coords.y;
        coords.x=1.-coords.x;
        vec2 dCoords = smoothstep(0.2, 0.6, abs(vec2(0.5, 0.5) - vertex_tex.xy));
        float screenEdgefactor = clamp(1.0 - (dCoords.x + dCoords.y), 0.0, 1.0);
        float ReflectionMultiplier = pow(Metallic, reflectionSpecularFalloffExponent) *
                    screenEdgefactor *
                    -reflected.z; 
        vec3 SSR = textureLod(screenTexture, coords.xy, 0).rgb * Fresnel;  
        color += vec4(SSR, Metallic)*0.4*reflecton;
    }
    else
    {
        color.rgb = texture(screenTexture, vertex_tex).rgb;
        color.a = 1;
    }
}
