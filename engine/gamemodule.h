#pragma once

#include "game.h"
#include "entity.h"
#include <ltdl.h>


class GameInterface;


class GameModule
{
public:
    bool    InitModule(const char* gamePath, GameInterface& interface);

    InitProc            Init = nullptr;
    RunProc             Run = nullptr;
    ChangeMapProc       ChangeMap = nullptr;
    DestroyProc         Destroy = nullptr;
    GetVariablesProc    GetVariables = nullptr;

    void    Touch(EntPtr entity, EntPtr other);
    void    Use(EntPtr entity, EntPtr other);
    void    Think(EntPtr entity);
    void    Blocked(EntPtr entity, EntPtr by);

private:
    bool    SetFunctions();

private:
    lt_dlhandle     handle = nullptr;
};


extern Variables globals;