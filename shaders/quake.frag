#version 330 core

out vec4 fragColor;


in vec2 uv;
in vec3 color;


uniform sampler2D   tex;


void main()
{
    vec3 color = texture(tex, uv).rgb;
    float r = 0;
    fragColor = vec4(color * (vec3(1) - vec3(r, r, r)), 1.0);
}
