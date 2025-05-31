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
    float       range;
    int         style;
    float       padding[2];
};

uniform int     numLights;
layout (std430, binding = 1) buffer Lights
{
    Light   lights[];
} lbuffer;


vec3 lightPos = vec3(512, 1132, 372);

vec3 CalculateBlinnPhong(vec3 color, vec3 normal, vec3 lightDir, vec3 lightColor, float intensity)
{
    //vec3 viewDir    = normalize(vec3(sceneData.viewPos) - inFragPos);
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

	float lightValue = max(dot(normal, lightDir), 0.0f);

    return color * (lightValue * intensity /* + lightColor * intensity * spec * .8 */);
}

void main()
{
    vec3 color = texture(tex, uv).rgb;
    vec3 ambient = color * 0.2;

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; ++i) {
        // // calculate per-light radiance
        // const Light light = PushConstants.lightsBuffer.lights[i];
        // vec3 pos = light.position;
        // float distance = length(pos - inFragPos);
        // if (distance > light.range) {
        //     continue;
        // }
        float distance = length(lbuffer.lights[i].origin - fragPos);
        if (distance > lbuffer.lights[i].range * 2) {
            continue;
        }
        vec3 L = normalize(lbuffer.lights[i].origin - fragPos);
        //float attenuation = CalculateAttenuation(lbuffer.lights[i].range, distance);
        Lo += CalculateBlinnPhong(color, normalize(normal), L, vec3(1, 0.8, 1)/* vec3(light.color) */, max(0, lbuffer.lights[i].intensity - (distance / 150)));
    }   

    fragColor = vec4(ambient + Lo, 1.0);
}
