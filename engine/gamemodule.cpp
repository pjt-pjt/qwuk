#include "gamemodule.h"
#include "gameinterface.h"


static Functions   functions;


bool    GameModule::Init(const char* gamePath, GameInterface& interface)
{
    lt_dlinit();
    interface.Init(&functions);

    handle = lt_dlopen(gamePath);
    bool ok = (handle != NULL);
    ok = ok && SetFunctions();
    ok = ok && (GameInit(&functions) == INIT_OK);
    return ok;
}

void    GameModule::Start(const char* startMap)
{
    GameStart(startMap);
}

void    GameModule::Destroy()
{
    GameDestroy();
    lt_dlclose(handle);
    lt_dlexit();
}

bool    GameModule::SetFunctions()
{
    GameInit = (InitProc)lt_dlsym(handle, "Init");
    GameStart = (StartProc)lt_dlsym(handle, "Start");
    GameDestroy = (DestroyProc)lt_dlsym(handle, "Destroy");
    return GameInit != nullptr && GameStart != nullptr && GameDestroy != nullptr;
}