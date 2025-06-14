#pragma once

#include "game.h"
#include <ltdl.h>


class GameInterface;


class GameModule
{
public:
    bool    Init(const char* gamePath, GameInterface& interface);

    void    Start(const char* startMap);
    void    Destroy(void);

    void    Collision(int entityIdx);

private:
    bool    SetFunctions();

private:
    lt_dlhandle     handle = nullptr;
    InitProc        GameInit = nullptr;
    StartProc       GameStart = nullptr;
    CollisionProc   GameCollision = nullptr;
    DestroyProc     GameDestroy = nullptr;
};
