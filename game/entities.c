#include "entities.h"
#include "tools.h"
#include "game.h"
#include <stdio.h>


void    InfoPlayerStart(Entity* self);
void    TriggerTeleport(Entity* self);
void    TriggerChangelevel(Entity* self);


void    Construct(Entity* entity)
{
    const char* Classes[] = {
        "info_player_start",
        "trigger_teleport",
        "trigger_changelevel",
        NULL
    };
    void (*Constructors[])(Entity* self) = {
        InfoPlayerStart,
        TriggerTeleport,
        TriggerChangelevel
    };

    for (int cl = 0; Classes[cl] != NULL; ++cl) {
        if (StrEq(entity->className, Classes[cl])) {
            Constructors[cl](entity);
            return;
        }
    }

    //Hack construct all necessary
    if (StrEq(entity->className, "worldspawn") ||
        StrPrefix(entity->className, "info_") ||
        StrPrefix(entity->className, "trigger_") ||
        StrPrefix(entity->className, "func_"))
    {
        i.Spawn(entity);
    }
}


void    InfoPlayerStart(Entity* self)
{
    EntPtr ent = i.Spawn(self);
    SetVec3(ent->mins, -16, -16, -24);
    SetVec3(ent->maxs,  16,  16,  32);
    ent->eyePos = 22;
    i.SpawnPlayer(ent);
}

void    TriggerTeleport(Entity* self)
{
    i.Spawn(self);
}
void    TriggerChangelevel(Entity* self)
{
    i.Spawn(self);
}
