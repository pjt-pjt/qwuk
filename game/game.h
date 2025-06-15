#pragma once


#if defined(__cplusplus)
    extern "C" {
#endif


typedef struct Functions
{
    void        (*PostCommand)(int command, const char* strParam1, float fltParam1, int intParam1);
    const char* (*EntityClass)(int entity);
    const char* (*EntityValueStr)(int entity, const char* key);
    int         (*EntityValueFloat)(int entity, const char* key, float* value);
    int         (*EntityValueVec3)(int entity, const char* key, float* value);
    int         (*SearchEntity)(const char* className, const char* key, const char* value);
    void        (*SpawnPlayer)(int entity);
    void        (*TeleportPlayer)(const float* origin, float angle);
} Functions;


#define INIT_OK 0


typedef int     (*InitProc)(Functions* functions);
typedef void    (*RunProc)(const char* startMap);
typedef void    (*ChangeMapProc)(void);
typedef void    (*CollisionProc)(int entityIdx);
typedef void    (*DestroyProc)(void);


#if defined(__cplusplus)
    }
#endif
