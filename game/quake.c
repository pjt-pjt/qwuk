#define TOOLS_IMPL
#include "tools.h"

#include "game.h"
#include "entities.h"


Interface   i;
Variables*  globals;


int     Init(Interface* interface, Variables* variables)
{
    i.EnumerateEntites = interface->EnumerateEntites;
    i.SearchEntity = interface->SearchEntity;

    i.EntityValueStr = interface->EntityValueStr;
    i.EntityValueInt = interface->EntityValueInt;
    i.EntityValueFloat = interface->EntityValueFloat;
    i.EntityValueVec3 = interface->EntityValueVec3;

    i.PostCommand = interface->PostCommand;
    i.Spawn = interface->Spawn;
    i.SpawnPlayer = interface->SpawnPlayer;
    i.SetAngle = interface->SetAngle;
    i.SetOrigin = interface->SetOrigin;
    i.SetPlayerOrigin = interface->SetPlayerOrigin;
    i.SetPlayerAngle = interface->SetPlayerAngle;

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
    ResetFields();
    EntPtr entity = i.EnumerateEntites(NULL);
    while (entity != NULL) {
        Construct(entity);
        entity = i.EnumerateEntites(entity);
    }
}

void    Destroy()
{
}

Variables*  GetVariables()
{
    return globals;
}