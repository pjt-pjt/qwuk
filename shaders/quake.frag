#version 330 core

out vec4 fragColor;


in vec2 uv;
in vec2 uv2;
in vec3 color;


uniform sampler2D   tex;
uniform sampler2D   lightmap;


void main()
{
    vec3 color = texture(tex, uv).rgb;
    float r = texture(lightmap, uv2).r;
    fragColor = vec4(color * (vec3(1) - vec3(r, r, r)), 1.0);
}
