#version 430 core

out vec4 fragColor;


layout (location = 0) in vec2 uv;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 color;
layout (location = 3) in vec3 fragPos;

uniform sampler2D   tex;


void main()
{
    vec3 color = texture(tex, uv).rgb;
    fragColor = vec4(color, 1.0);
}
