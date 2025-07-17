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

typedef union Fields
{
    struct DoorFields
    {
        Vec3        size;
        Vec3        direction;
        Vec3        pos1;
        Vec3        pos2;
        float       speed;
    };
} Fields;

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
    void            (*Use)(struct Entity* self, struct Entity* other);
    void            (*Think)(struct Entity* self);
    Fields*         f;
    int             flags;
    int             wait;
    struct Entity*  owner;
    struct Entity*  link;
} Entity;

typedef Entity* EntPtr;

#if defined(__cplusplus)
    }
#endif
