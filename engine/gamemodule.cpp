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

void    GameModule::Run(const char* startMap)
{
    GameRun(startMap);
}

void    GameModule::Destroy()
{
    GameDestroy();
    lt_dlclose(handle);
    lt_dlexit();
}

void    GameModule::Collision(int entityIdx)
{
    GameCollision(entityIdx);
}

bool    GameModule::SetFunctions()
{
    GameInit = (InitProc)lt_dlsym(handle, "Init");
    GameRun = (RunProc)lt_dlsym(handle, "Run");
    GameCollision = (CollisionProc)lt_dlsym(handle, "Collision");
    GameDestroy = (DestroyProc)lt_dlsym(handle, "Destroy");
    return GameInit != nullptr && GameRun != nullptr &&
           GameCollision != nullptr && GameDestroy != nullptr;
}