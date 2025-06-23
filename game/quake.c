#include "game.h"
#include "entities.h"
#include <stdio.h>

#define TOOLS_IMPL
#include "tools.h"


Interface   i;
Variables*  globals;


int     Init(Interface* interface, Variables* variables)
{
    i.EnumerateEntites = interface->EnumerateEntites;
    i.SearchEntity = interface->SearchEntity;

    i.EntityValueStr = interface->EntityValueStr;
    i.EntityValueFloat = interface->EntityValueFloat;
    i.EntityValueVec3 = interface->EntityValueVec3;

    i.PostCommand = interface->PostCommand;
    i.Spawn = interface->Spawn;
    i.SpawnPlayer = interface->SpawnPlayer;
    i.SetPlayerPosAngle = interface->SetPlayerPosAngle;

    globals = variables;
    return INIT_OK;
}

void    Run(char* startMap)
{
    if (startMap == NULL) {
        i.PostCommand(1, "maps/start.bsp", 0, 0);
    } else {
        i.PostCommand(1, startMap, 0, 0);
    }
}

void    ChangeMap(void)
{
    EntPtr entity = i.EnumerateEntites(NULL);
    while (entity != NULL) {
        Construct(entity);
        entity = i.EnumerateEntites(entity);
    }
}

void    Touch(EntPtr entity, EntPtr other)
{
    if (entity->Touch != NULL) {
        entity->Touch(entity, other);
    }
}

void    Destroy()
{
}
