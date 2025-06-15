#pragma once

#include "game.h"
#include <ltdl.h>


class GameInterface;


class GameModule
{
public:
    bool    Init(const char* gamePath, GameInterface& interface);

    void    Run(const char* startMap);
    void    Destroy(void);

    void    Collision(int entityIdx);

private:
    bool    SetFunctions();

private:
    lt_dlhandle     handle = nullptr;
    InitProc        GameInit = nullptr;
    RunProc         GameRun = nullptr;
    CollisionProc   GameCollision = nullptr;
    DestroyProc     GameDestroy = nullptr;
};
