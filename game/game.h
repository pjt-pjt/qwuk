#pragma once

#include "entity.h"

#if defined(__cplusplus)
    extern "C" {
#endif


typedef struct Variables
{
    int         status;
    const char* map;
} Variables;


typedef struct Interface
{
    EntPtr      (*EnumerateEntites)(EntPtr from);
    EntPtr      (*SearchEntity)(const char* className, const char* key, const char* value);

    const char* (*EntityValueStr)(EntPtr entity, const char* key);
    int         (*EntityValueFloat)(EntPtr entity, const char* key, float* value);
    int         (*EntityValueVec3)(EntPtr entity, const char* key, Vec3 value);

    void        (*PostCommand)(int command, const char* strParam1, float fltParam1, int intParam1);
    EntPtr      (*Spawn)(EntPtr entity);
    void        (*SpawnPlayer)(EntPtr entity);
    void        (*SetPlayerPosAngle)(const Vec3 origin, float angle);
} Interface;

extern Interface i;


#define INIT_OK 0

typedef int     (*InitProc)(Interface* interface, Variables* variables);
typedef void    (*RunProc)(const char* startMap);
typedef void    (*ChangeMapProc)(void);
typedef void    (*TouchProc)(EntPtr entity, EntPtr other);
typedef void    (*DestroyProc)(void);


#if defined(__cplusplus)
    }
#endif
