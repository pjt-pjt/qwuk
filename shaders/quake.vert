#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec2 aUV2;
layout (location = 3) in vec3 aColor;

out vec2 uv;
out vec2 uv2;
out vec3 color;


uniform mat4 view;
uniform mat4 proj;


void main()
{
    uv = aUV;
    uv2 = aUV2;
    color = aColor;
    gl_Position = proj * view * vec4(aPos, 1.0);
}