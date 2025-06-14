#include "game.h"
#include <stdio.h>
#include <string.h>


static Functions* gFunctions;

int     Init(Functions* functions)
{
    gFunctions = functions;
    return INIT_OK;
}

void    Start(char* startMap)
{
    if (startMap == NULL) {
        gFunctions->PostCommand(1, "maps/start.bsp", 0, 0);
    } else {
        gFunctions->PostCommand(1, startMap, 0, 0);
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
            gFunctions->SetPlayer(origin, angle);
        }
    }
}


void    Destroy()
{
}