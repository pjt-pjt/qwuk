#pragma once

#include <string.h>


int     StrEq(const char* left, const char* right);
int     StrPrefix(const char* str, const char* subStr);

void    Vec3Set(Vec3 vec3, float x, float y, float z);
void    Vec3Copy(Vec3 target, const Vec3 source);
void    Vec3Add(Vec3 res, const Vec3 v1, const Vec3 v2);
void    Vec3Sub(Vec3 res, const Vec3 v1, const Vec3 v2);
void    Vec3Mul(Vec3 res, const Vec3 v, float mul);
float   Vec3Dot(const Vec3 v1, const Vec3 v2);


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

    void    Vec3Set(Vec3 vec3, float x, float y, float z)
    {
        vec3[0] = x; vec3[1] = y; vec3[2] = z;
    }
    void    Vec3Copy(Vec3 target, const Vec3 source)
    {
        memcpy(target, source, 3 * sizeof(float));
    }
    void    Vec3Add(Vec3 res, const Vec3 v1, const Vec3 v2)
    {
        res[0] = v1[0] + v2[0];
        res[1] = v1[1] + v2[1];
        res[2] = v1[2] + v2[2];
    }
    void    Vec3Sub(Vec3 res, const Vec3 v1, const Vec3 v2)
    {
        res[0] = v1[0] - v2[0];
        res[1] = v1[1] - v2[1];
        res[2] = v1[2] - v2[2];
    }
    void    Vec3Mul(Vec3 res, const Vec3 v, float mul)
    {
        res[0] = v[0] * mul;
        res[1] = v[1] * mul;
        res[2] = v[2] * mul;
    }
    float   Vec3Dot(const Vec3 v1, const Vec3 v2)
    {
        return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
    }

#endif


#define UNUSED(x) (void)(x)
