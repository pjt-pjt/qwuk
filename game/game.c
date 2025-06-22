#include "game.h"
#include "entities.h"
#include <stdio.h>

#define TOOLS_IMPL
#include "tools.h"


Interface i;

int     Init(Interface* interface)
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

void    Collision(EntPtr entity)
{
    if (strcmp(entity->className, "trigger_teleport") == 0) {
        const char* target = i.EntityValueStr(entity, "target");
        EntPtr targetEnt = i.SearchEntity("info_teleport_destination", "targetname", target);
        if (targetEnt != NULL) {
            Vec3 origin;
            CopyVec3(origin, targetEnt->origin);
            float angle = targetEnt->angle;
            origin[2] -= targetEnt->mins[2];
            i.SetPlayerPosAngle(origin, angle);
        }
    } else if (strcmp(entity->className, "trigger_changelevel") == 0) {
        const char* map = i.EntityValueStr(entity, "map");
        char path[1024] = "";
        strcat(path, "maps/");
        strcat(path, map);
        strcat(path, ".bsp");
        i.PostCommand(1, path, 0, 0);
    }
}

void    Destroy()
{
}
