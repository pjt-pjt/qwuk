#pragma once

#include <stdint.h>
#include <string.h>


struct Edict
{
    const char*     key;
    const char*     value;
    struct Edict*   next = NULL;
};


struct Entity_
{
    const char*     className;
    float           origin[3] = {0, 0, 0};
    float           angle = 0;
    int32_t         model = -1;
    const Edict*    first = NULL;
};


bool Equals(const char* left, const char* right);
bool StartsWith(const char* str, const char* subStr);
