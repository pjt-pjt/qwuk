#version 330 core

out vec4 fragColor;


in vec2 uv;
in vec3 color;


uniform int         testMode;
uniform sampler2D   tex;


void main()
{
    if (testMode == 2/*Surfaces*/) {
        fragColor = vec4(color, 1.0) * 2.5;
    } else {
        fragColor = texture(tex, uv);
    }
}
