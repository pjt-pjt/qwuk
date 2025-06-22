#include "gamemodule.h"
#include "gameinterface.h"


static Interface   interface;

bool    GameModule::InitModule(const char* gamePath, GameInterface& game)
{
    lt_dlinit();
    game.Init(&interface);

    handle = lt_dlopen(gamePath);
    bool ok = (handle != nullptr);
    ok = ok && SetFunctions();
    ok = ok && (Init(&interface) == INIT_OK);
    return ok;
}

bool    GameModule::SetFunctions()
{
    Init = (InitProc)lt_dlsym(handle, "Init");
    Run = (RunProc)lt_dlsym(handle, "Run");
    ChangeMap = (ChangeMapProc)lt_dlsym(handle, "ChangeMap");
    Touch = (TouchProc)lt_dlsym(handle, "Touch");
    Destroy = (DestroyProc)lt_dlsym(handle, "Destroy");
    return Init != nullptr && Run != nullptr && ChangeMap != nullptr &&
           Touch != nullptr && Destroy != nullptr;
}