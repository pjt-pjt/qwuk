#pragma once


#if defined(__cplusplus)
    extern "C" {
#endif


typedef const void* EntPtr;

typedef struct Functions
{
    void        (*PostCommand)(int command, const char* strParam1, float fltParam1, int intParam1);
    EntPtr      (*EnumerateEntites)(EntPtr from);
    const char* (*EntityClass)(EntPtr entity);
    const char* (*EntityValueStr)(EntPtr entity, const char* key);
    int         (*EntityValueFloat)(EntPtr entity, const char* key, float* value);
    int         (*EntityValueVec3)(EntPtr entity, const char* key, float* value);
    EntPtr      (*SearchEntity)(const char* className, const char* key, const char* value);
    void        (*SpawnPlayer)(EntPtr entity);
    void        (*TeleportPlayer)(const float* origin, float angle);
} Functions;


#define INIT_OK 0

typedef int     (*InitProc)(Functions* functions);
typedef void    (*RunProc)(const char* startMap);
typedef void    (*ChangeMapProc)(void);
typedef void    (*CollisionProc)(EntPtr entity);
typedef void    (*DestroyProc)(void);


#if defined(__cplusplus)
    }
#endif
