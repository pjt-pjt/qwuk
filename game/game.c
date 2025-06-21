#include "game.h"
#include <stdio.h>

#define TOOLS_IMPL
#include "tools.h"


static Interface i;

int     Init(Interface* interface)
{
    i.EnumerateEntites = interface->EnumerateEntites;
    i.SearchEntity = interface->SearchEntity;

    i.EntityClass = interface->EntityClass;
    i.EntityValueStr = interface->EntityValueStr;
    i.EntityValueFloat = interface->EntityValueFloat;
    i.EntityValueVec3 = interface->EntityValueVec3;

    i.SetEntityFloat = interface->SetEntityFloat;
    i.SetEntityVec3 = interface->SetEntityVec3;

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
        if (StrEq(i.EntityClass(entity), "worldspawn")) {
            i.Spawn(entity);
        } else if (StrEq(i.EntityClass(entity), "info_player_start")) {
            EntPtr ent = i.Spawn(entity);
            Vec3 mins, maxs;
            SetVec3(mins, -16, -16, -24);
            SetVec3(maxs,  16,  16,  32);
            i.SetEntityVec3(ent, "mins", mins);
            i.SetEntityVec3(ent, "maxs", maxs);
            i.SetEntityFloat(ent, "eyePos", 22);
            i.SpawnPlayer(ent);
        } else if (StrPrefix(i.EntityClass(entity), "info_") ||
                   StrPrefix(i.EntityClass(entity), "trigger_") ||
                   StrPrefix(i.EntityClass(entity), "func_"))
        {
            i.Spawn(entity);
        }
        entity = i.EnumerateEntites(entity);
    }
}

void    Collision(EntPtr entity)
{
    if (strcmp(i.EntityClass(entity), "trigger_teleport") == 0) {
        const char* target = i.EntityValueStr(entity, "target");
        EntPtr targetEnt = i.SearchEntity("info_teleport_destination", "targetname", target);
        if (targetEnt != NULL) {
            float origin[3];
            i.EntityValueVec3(targetEnt, "origin", origin);
            float angle;
            i.EntityValueFloat(targetEnt, "angle", &angle);
            Vec3 mins;
            i.EntityValueVec3(targetEnt, "mins", mins);
            origin[2] -= mins[2];
            i.SetPlayerPosAngle(origin, angle);
        }
    } else if (strcmp(i.EntityClass(entity), "trigger_changelevel") == 0) {
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
