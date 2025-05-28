#version 330 core

out vec4 fragColor;


in vec2 uv;
in vec3 normal;
in vec3 color;


uniform sampler2D   tex;


void main()
{
    vec3 color = texture(tex, uv).rgb;
    fragColor = vec4(color, 1.0);
}
