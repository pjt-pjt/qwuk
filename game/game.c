#include "game.h"
#include <stdio.h>


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
    entityIdx = entityIdx;
}


void    Destroy()
{
}