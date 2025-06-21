#pragma once

#include <cstdint>


struct Edict
{
    const char*     key;
    const char*     value;
    struct Edict*   next = nullptr;
};


struct Entity
{
    const char*     className;
    float           origin[3] = {0, 0, 0};
    float           angle = 0;
    int32_t         model = -1;
    const Edict*    first = nullptr;
};
