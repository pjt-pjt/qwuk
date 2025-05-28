#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;

out vec2 uv;
out vec3 normal;
out vec3 color;


uniform mat4 view;
uniform mat4 proj;
uniform int  testMode;
uniform vec3 faceColor;


void main()
{
    uv = aUV;
    if (testMode == 1/*Surfaces*/) {
        color = faceColor;
    } else {
        color = aColor;
    }
    normal = aNormal;
    gl_Position = proj * view * vec4(aPos, 1.0);
}
