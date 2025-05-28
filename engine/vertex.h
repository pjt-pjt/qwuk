#pragma once

#include "glm/glm.hpp"
#include <cstdint>


struct Color
{
    float b;
    float g;
    float r;
    Color(uint8_t r = 0, uint8_t g = 0, uint8_t b = 0) :
        b(float(b) / 255.0),
        g(float(g) / 255.0),
        r(float(r) / 255.0)
    {}
    Color(uint32_t color)
    {
        b = float(color & 0xff) / 255.0;
        g = float((color >> 8) & 0xff) / 255.0;
        r = float((color >> 16) & 0xff) / 255.0;
    }
    Color(const Color& other) :
        b(other.b),
        g(other.g),
        r(other.r)
    {}
    operator uint32_t()
    {
        uint32_t color = uint32_t(r * 255.0);
        color <<= 8;
        color |= uint32_t(g * 255.0);
        color <<= 8;
        color |= uint32_t(b * 255.0);
        return color;
    }
    operator glm::vec3()
    {
        return glm::vec3(r, g, b);
    }

    Color& operator=(const Color&) = default;
    Color& operator=(Color&&) = default;
};

struct Vertex
{
    glm::vec3   pos;
    glm::vec2   uv;
    glm::vec3   normal;
    Color       color;
};
