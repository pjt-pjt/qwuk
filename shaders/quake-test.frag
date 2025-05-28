#version 330 core

out vec4 fragColor;


in vec2 uv;
in vec3 normal;
in vec3 color;


uniform int         testMode;
uniform sampler2D   tex;


void main()
{
    if (testMode == 1/*Surfaces*/) {
        fragColor = vec4(color, 1.0);
    } else {
        fragColor = texture(tex, uv) * 1.5;
    }
}
