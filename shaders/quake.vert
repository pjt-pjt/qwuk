#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aUV;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec3 aColor;

layout (location = 0) out vec2 uv;
layout (location = 1) out vec3 normal;
layout (location = 2) out vec3 color;
layout (location = 3) out vec3 fragPos;


uniform mat4 view;
uniform mat4 proj;


void main()
{
    uv = aUV;
    normal = aNormal;
    color = aColor;
    fragPos = /* model * */aPos;
    gl_Position = proj * view * vec4(fragPos, 1.0);
}