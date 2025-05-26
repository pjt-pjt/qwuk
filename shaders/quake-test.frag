#version 330 core

out vec4 fragColor;


in vec2 uv;
in vec2 uv2;
in vec3 color;


uniform int         testMode;
uniform sampler2D   tex;
uniform sampler2D   lightmap;


void main()
{
    if (testMode == 2/*Surfaces*/) {
        fragColor = vec4(color, 1.0) * 2.5;
    } else if (testMode == 1/*lightmap*/) {
        float r = texture(lightmap, uv2).r;
        fragColor = vec4(r, r, r, 1.0);
    } else {
        fragColor = texture(tex, uv);
    }
}
