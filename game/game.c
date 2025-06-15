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
    const char* className;
    int entity = 0;
    while ((className = gFunctions->EntityClass(entity)) != NULL) {
        if (strcmp(gFunctions->EntityClass(entity), "info_player_start") == 0) {
            gFunctions->SpawnPlayer(entity);
            break;  //TODO
        }    
        ++entity;
    }
}

void    Collision(int entityIdx)
{
    if (strcmp(gFunctions->EntityClass(entityIdx), "trigger_teleport") == 0) {
        const char* target = gFunctions->EntityValueStr(entityIdx, "target");
        int targetIdx = gFunctions->SearchEntity("info_teleport_destination", "targetname", target);
        if (targetIdx != -1) {
            float origin[3];
            gFunctions->EntityValueVec3(targetIdx, "origin", origin);
            float angle;
            gFunctions->EntityValueFloat(targetIdx, "angle", &angle);
            gFunctions->TeleportPlayer(origin, angle);
        }
    } else if (strcmp(gFunctions->EntityClass(entityIdx), "trigger_changelevel") == 0) {
        const char* map = gFunctions->EntityValueStr(entityIdx, "map");
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
