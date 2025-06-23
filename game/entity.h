#pragma once

#if defined(__cplusplus)
    extern "C" {
#endif


typedef struct Edict
{
    const char*     key;
    const char*     value;
    struct Edict*   next;
} Edict;


typedef float Vec3[3];

typedef struct Entity
{
    const char*     className;
    const Edict*    first;
    Vec3            origin;
    float           angle;
    int             model;
    float           eyePos;
    Vec3            mins;
    Vec3            maxs;
    void            (*Touch)(struct Entity* self, struct Entity* other);
} Entity;

typedef Entity* EntPtr;

#if defined(__cplusplus)
    }
#endif
