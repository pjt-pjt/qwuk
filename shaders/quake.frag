#version 430 core

out vec4 fragColor;


layout (location = 0) in vec2 uv;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 fragPos;

uniform sampler2D   tex;

int  numLights = 1;
vec3 lightPos = vec3(512, 1132, 372);

vec3 CalculateBlinnPhong(vec3 color, vec3 normal, vec3 lightDir, vec3 lightColor, float intensity)
{
    //vec3 viewDir    = normalize(vec3(sceneData.viewPos) - inFragPos);
    //vec3 halfwayDir = normalize(lightDir + viewDir);
    //float spec = pow(max(dot(normal, halfwayDir), 0.0), 16.0);

	float lightValue = max(dot(normal, lightDir), 0.0f);

    return color * (lightValue * intensity /* + lightColor * intensity * spec * .8 */);
}

float CalculateAttenuation(float range, float distance)
{
    float distanceSq = distance * distance;
    float rangeQ = range * range * range * range;
    return max(min(1.0 - ((distanceSq * distanceSq) / rangeQ), 1), 0) / distanceSq;
}

void main()
{
    vec3 color = texture(tex, uv).rgb;
    vec3 ambient = color * 0.4;

    vec3 Lo = vec3(0.0);
    for (int i = 0; i < numLights; ++i) {
        // // calculate per-light radiance
        // const Light light = PushConstants.lightsBuffer.lights[i];
        // vec3 pos = light.position;
        // float distance = length(pos - inFragPos);
        // if (distance > light.range) {
        //     continue;
        // }
        float distance = length(lightPos - fragPos);
        vec3 L = normalize(lightPos - fragPos);
        float attenuation = CalculateAttenuation(1024, distance);
        Lo += CalculateBlinnPhong(color, normal, L, vec3(1, 1, 1)/* vec3(light.color) */, 3 * attenuation);
    }   

    fragColor = vec4(ambient + Lo, 1.0);
}
