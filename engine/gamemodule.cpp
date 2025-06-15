#include "gamemodule.h"
#include "gameinterface.h"


static Functions   functions;


bool    GameModule::InitModule(const char* gamePath, GameInterface& interface)
{
    lt_dlinit();
    interface.Init(&functions);

    handle = lt_dlopen(gamePath);
    bool ok = (handle != NULL);
    ok = ok && SetFunctions();
    ok = ok && (Init(&functions) == INIT_OK);
    return ok;
}

bool    GameModule::SetFunctions()
{
    Init = (InitProc)lt_dlsym(handle, "Init");
    Run = (RunProc)lt_dlsym(handle, "Run");
    ChangeMap = (ChangeMapProc)lt_dlsym(handle, "ChangeMap");
    Collision = (CollisionProc)lt_dlsym(handle, "Collision");
    Destroy = (DestroyProc)lt_dlsym(handle, "Destroy");
    return Init != nullptr && Run != nullptr && ChangeMap != nullptr &&
           Collision != nullptr && Destroy != nullptr;
}