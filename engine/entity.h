#pragma once

#include <cstdint>


struct Edict
{
    const char*     key;
    const char*     value;
    struct Edict*   next = nullptr;
};


typedef float Vec3[3];

struct Entity
{
    const char*     className;
    const Edict*    first = nullptr;
    Vec3            origin = {0, 0, 0};
    float           angle = 0;
    int32_t         model = -1;
    float           eyePos = 0;
    Vec3            mins = {0, 0, 0};
    Vec3            maxs = {0, 0, 0};
};
