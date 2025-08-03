#pragma once

#include "game.h"
#include "entity.h"


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
    static EntPtr       SearchEntity(EntPtr from, const char* className, const char* key, const char* value);

    static const char*  EntityValueStr(EntPtr entity, const char* key);
    static int          EntityValueInt(EntPtr entity, const char* key, int* value);
    static int          EntityValueFloat(EntPtr entity, const char* key, float* value);
    static int          EntityValueVec3(EntPtr entity, const char* key, float* value);

    static void         PostCommand(int command, const char* strParam1, float fltParam1, int intParam1);
    static EntPtr       Spawn(EntPtr entity);
    static void         SpawnPlayer(EntPtr entity);
    static void         SetOrigin(EntPtr entity, const Vec3 origin);
    static void         MoveTo(EntPtr entity, const Vec3 origin);
    static void         SetAngle(EntPtr entity, float angle);
    static void         SetPlayerOrigin(const Vec3 origin);
    static void         SetPlayerAngle(float angle);

private:
    Quake&      quake;
    BSP&        bsp;
};
