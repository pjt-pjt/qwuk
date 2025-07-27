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
    Destroy = (DestroyProc)lt_dlsym(handle, "Destroy");
    GetVariables = (GetVariablesProc)lt_dlsym(handle, "GetVariables");
    return Init != nullptr && Run != nullptr && ChangeMap != nullptr &&
           GetVariables != nullptr && Destroy != nullptr;
}


void    GameModule::Touch(EntPtr entity, EntPtr other)
{
    if (entity->Touch != NULL) {
        entity->Touch(entity, other);
    }
}

void    GameModule::Use(EntPtr entity, EntPtr other)
{
    if (entity->Use != NULL && entity->playerUse) {
        entity->Use(entity, other);
    }
}

void    GameModule::Think(EntPtr entity)
{
    if (entity->Think != NULL) {
        entity->Think(entity);
    }
}

void    GameModule::Blocked(EntPtr entity, EntPtr by)
{
    if (entity->Blocked != NULL) {
        entity->Blocked(entity, by);
    }
}
