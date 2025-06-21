#pragma once

#include "game.h"


class Quake;
class BSP;
struct Interface;

class GameInterface
{
public:
    GameInterface(Quake& quake);
    ~GameInterface();

    void    Init(Interface* interface);

private:
    static EntPtr       EnumerateEntites(EntPtr from);
    static EntPtr       SearchEntity(const char* className, const char* key, const char* value);

    static const char*  EntityClass(EntPtr entity);
    static const char*  EntityValueStr(EntPtr entity, const char* key);
    static int          EntityValueFloat(EntPtr entity, const char* key, float* value);
    static int          EntityValueVec3(EntPtr entity, const char* key, float* value);

    static void         SetEntityFloat(EntPtr entity, const char* member, float value);
    static void         SetEntityVec3(EntPtr entity, const char* member, Vec3 vec3);

    static void         PostCommand(int command, const char* strParam1, float fltParam1, int intParam1);
    static EntPtr       Spawn(EntPtr entity);
    static void         SpawnPlayer(EntPtr entity);
    static void         SetPlayerPosAngle(const Vec3 origin, float angle);

private:
    Quake&      quake;
    BSP&        bsp;
};
