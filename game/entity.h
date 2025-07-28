#pragma once

#if defined(__cplusplus)
    extern "C" {
#endif


#include "tools.h"


typedef struct Edict
{
    const char*     key;
    const char*     value;
    struct Edict*   next;
} Edict;


typedef union Fields
{
    struct Common
    {
        Vec3        size;
        Vec3        direction;
        Vec3        pos1;
        Vec3        pos2;
        float       speed;
        float       wait;
    };
    struct DoorFields
    {
        struct Common commonDoor;
        #define DOOR_CLOSED 0
        #define DOOR_OPEN   1
        int         doorStatus;
    };
    struct ButtonFields
    {
        struct Common commonButton;
        #define BUTTON_WAIT     0
        #define BUTTON_PRESSED  1
        int         buttonStatus;
    };
    struct PlatFields
    {
        struct Common commonPlat;
        #define PLAT_TOP        0
        #define PLAT_BOTTOM     1
        int         platStatus;
    };
    struct CounterFields
    {
        int         counter;
        int         count;
    };
} Fields;

typedef struct Entity
{
    const char*     className;
    const Edict*    first;
    Vec3            origin;
    Vec3            prevOrigin;
    float           angle;
    int             model;
    float           eyePos;
    Vec3            mins;
    Vec3            maxs;
    void            (*Touch)(struct Entity* self, struct Entity* other);
    void            (*Use)(struct Entity* self, struct Entity* other);
    void            (*Think)(struct Entity* self);
    void            (*Blocked)(struct Entity* self, struct Entity* by);
    Fields*         f;
    int             flags;
    float           sleep;
    int             playerUse;
    struct Entity*  owner;
    struct Entity*  link;
} Entity;

typedef Entity* EntPtr;

#if defined(__cplusplus)
    }
#endif
