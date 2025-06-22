#pragma once

#include <string.h>


int     StrEq(const char* left, const char* right);
int     StrPrefix(const char* str, const char* subStr);
void    SetVec3(float* vec3, float x, float y, float z);
void    CopyVec3(float* target, const float* source);


#if defined(TOOLS_IMPL)

    int    StrEq(const char* left, const char* right)
    {
        return (strcmp(left, right) == 0);
    }
    int    StrPrefix(const char* str, const char* subStr)
    {
        int len1 = strlen(str);
        int len2 = strlen(subStr);
        return (len2 <= len1 && strncmp(str, subStr, len2) == 0);
    }
    void    SetVec3(float* vec3, float x, float y, float z)
    {
        vec3[0] = x; vec3[1] = y; vec3[2] = z;
    }
    void    CopyVec3(float* target, const float* source)
    {
        memcpy(target, source, 3 * sizeof(float));
    }

#endif
