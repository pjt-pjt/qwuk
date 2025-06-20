#include "game.h"
#include <stdio.h>
#include <string.h>


static Interface* gInterface;

int     Init(Interface* interface)
{
    gInterface = interface;
    return INIT_OK;
}

void    Run(char* startMap)
{
    if (startMap == NULL) {
        gInterface->PostCommand(1, "maps/start.bsp", 0, 0);
    } else {
        gInterface->PostCommand(1, startMap, 0, 0);
    }
}

void    ChangeMap(void)
{
    EntPtr entity = gInterface->EnumerateEntites(NULL);
    while (entity != NULL) {
        if (strcmp(gInterface->EntityClass(entity), "info_player_start") == 0) {
            gInterface->SpawnPlayer(entity);
            break;  //TODO
        }    
        entity = gInterface->EnumerateEntites(entity);
    }
}

void    Collision(EntPtr entity)
{
    if (strcmp(gInterface->EntityClass(entity), "trigger_teleport") == 0) {
        const char* target = gInterface->EntityValueStr(entity, "target");
        EntPtr targetEnt = gInterface->SearchEntity("info_teleport_destination", "targetname", target);
        if (targetEnt != NULL) {
            float origin[3];
            gInterface->EntityValueVec3(targetEnt, "origin", origin);
            float angle;
            gInterface->EntityValueFloat(targetEnt, "angle", &angle);
            gInterface->TeleportPlayer(origin, angle);
        }
    } else if (strcmp(gInterface->EntityClass(entity), "trigger_changelevel") == 0) {
        const char* map = gInterface->EntityValueStr(entity, "map");
        char path[1024] = "";
        strcat(path, "maps/");
        strcat(path, map);
        strcat(path, ".bsp");
        gInterface->PostCommand(1, path, 0, 0);
    }
}

void    Destroy()
{
}
