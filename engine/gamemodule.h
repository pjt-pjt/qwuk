#pragma once

#include "game.h"
#include <ltdl.h>


class GameInterface;


class GameModule
{
public:
    bool    InitModule(const char* gamePath, GameInterface& interface);

    InitProc            Init = nullptr;
    RunProc             Run = nullptr;
    ChangeMapProc       ChangeMap = nullptr;
    TouchProc           Touch = nullptr;
    UseProc             Use = nullptr;
    ThinkProc           Think = nullptr;
    DestroyProc         Destroy = nullptr;
    GetVariablesProc    GetVariables = nullptr;

private:
    bool    SetFunctions();

private:
    lt_dlhandle     handle = nullptr;
};


extern Variables globals;