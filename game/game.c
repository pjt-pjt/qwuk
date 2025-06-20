#include "game.h"
#include <stdio.h>
#include <string.h>


static Functions* gFunctions;

int     Init(Functions* functions)
{
    gFunctions = functions;
    return INIT_OK;
}

void    Run(char* startMap)
{
    if (startMap == NULL) {
        gFunctions->PostCommand(1, "maps/start.bsp", 0, 0);
    } else {
        gFunctions->PostCommand(1, startMap, 0, 0);
    }
}

void    ChangeMap(void)
{
    EntPtr entity = gFunctions->EnumerateEntites(NULL);
    while (entity != NULL) {
        if (strcmp(gFunctions->EntityClass(entity), "info_player_start") == 0) {
            gFunctions->SpawnPlayer(entity);
            break;  //TODO
        }    
        entity = gFunctions->EnumerateEntites(entity);
    }
}

void    Collision(EntPtr entity)
{
    if (strcmp(gFunctions->EntityClass(entity), "trigger_teleport") == 0) {
        const char* target = gFunctions->EntityValueStr(entity, "target");
        EntPtr targetEnt = gFunctions->SearchEntity("info_teleport_destination", "targetname", target);
        if (targetEnt != NULL) {
            float origin[3];
            gFunctions->EntityValueVec3(targetEnt, "origin", origin);
            float angle;
            gFunctions->EntityValueFloat(targetEnt, "angle", &angle);
            gFunctions->TeleportPlayer(origin, angle);
        }
    } else if (strcmp(gFunctions->EntityClass(entity), "trigger_changelevel") == 0) {
        const char* map = gFunctions->EntityValueStr(entity, "map");
        char path[1024] = "";
        strcat(path, "maps/");
        strcat(path, map);
        strcat(path, ".bsp");
        gFunctions->PostCommand(1, path, 0, 0);
    }
}

void    Destroy()
{
}
