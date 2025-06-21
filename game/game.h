#pragma once


#if defined(__cplusplus)
    extern "C" {
#endif


typedef void* EntPtr;
typedef float Vec3[3];

typedef struct Interface
{
    EntPtr      (*EnumerateEntites)(EntPtr from);
    EntPtr      (*SearchEntity)(const char* className, const char* key, const char* value);

    const char* (*EntityClass)(EntPtr entity);
    const char* (*EntityValueStr)(EntPtr entity, const char* key);
    int         (*EntityValueFloat)(EntPtr entity, const char* key, float* value);
    int         (*EntityValueVec3)(EntPtr entity, const char* key, Vec3 value);

    void        (*SetEntityFloat)(EntPtr entity, const char* member, float value);
    void        (*SetEntityVec3)(EntPtr entity, const char* member, Vec3 vec3);

    void        (*PostCommand)(int command, const char* strParam1, float fltParam1, int intParam1);
    EntPtr      (*Spawn)(EntPtr entity);
    void        (*SpawnPlayer)(EntPtr entity);
    void        (*TeleportPlayer)(const float* origin, float angle);
} Interface;


#define INIT_OK 0

typedef int     (*InitProc)(Interface* interface);
typedef void    (*RunProc)(const char* startMap);
typedef void    (*ChangeMapProc)(void);
typedef void    (*CollisionProc)(EntPtr entity);
typedef void    (*DestroyProc)(void);


#if defined(__cplusplus)
    }
#endif
