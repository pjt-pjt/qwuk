#include "gamemodule.h"
#include "gameinterface.h"


static Interface    interface;
Variables           globals;

bool    GameModule::InitModule(const char* gamePath, GameInterface& game)
{
    lt_dlinit();
    game.Init(&interface);

    handle = lt_dlopen(gamePath);
    bool ok = (handle != nullptr);
    ok = ok && SetFunctions();
    ok = ok && (Init(&interface, &globals) == INIT_OK);
    return ok;
}

bool    GameModule::SetFunctions()
{
    Init = (InitProc)lt_dlsym(handle, "Init");
    Run = (RunProc)lt_dlsym(handle, "Run");
    ChangeMap = (ChangeMapProc)lt_dlsym(handle, "ChangeMap");
    Touch = (TouchProc)lt_dlsym(handle, "Touch");
    Use = (UseProc)lt_dlsym(handle, "Use");
    Think = (ThinkProc)lt_dlsym(handle, "Think");
    Destroy = (DestroyProc)lt_dlsym(handle, "Destroy");
    GetVariables = (GetVariablesProc)lt_dlsym(handle, "GetVariables");
    return Init != nullptr && Run != nullptr && ChangeMap != nullptr &&
           Touch != nullptr && Use != nullptr  && Think != nullptr &&
           GetVariables != nullptr && Destroy != nullptr;
}