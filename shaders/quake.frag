#version 430 core

out vec4 fragColor;


layout (location = 0) in vec2 uv;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 fragPos;

uniform sampler2D   tex;

struct Light
{
    vec3        origin;
    float       intensity;
    vec3        color;
    float       range;
    int         style;
    float       padding[3];
};

uniform int     numLights;
layout (std430, binding = 1) buffer Lights
{
    Light   lights[];
} lbuffer;


uniform vec3    viewPos;

float intensityBase = 300;
float brightnessMultiplier = 0.75;
float gamma = 1.2;


vec3 CalculateBlinnPhong(vec3 color, vec3 normal, vec3 lightDir, vec3 lightColor, float intensity)
{
    vec3 viewDir    = normalize(vec3(viewPos) - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 8.0);

	float lightValue = max(dot(normal, lightDir), 0.0f);
    intensity = intensity / intensityBase * brightnessMultiplier;
    return color * (lightValue * intensity + lightColor * intensity * spec);
}

void main()
{
    vec3 color = texture(tex, uv).rgb;
    vec3 ambient = color * 0.2;

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; ++i) {
        const Light light = lbuffer.lights[i];
        float distance = length(light.origin - fragPos);
        if (distance > light.range) {
            continue;
        }
        vec3 L = normalize(light.origin - fragPos);
        Lo += CalculateBlinnPhong(color, normalize(normal), L, light.color, max(0, light.intensity - distance));
    }   

    fragColor = vec4(ambient + Lo, 1.0);
    fragColor.rgb = pow(fragColor.rgb, vec3(1.0/gamma));
}
