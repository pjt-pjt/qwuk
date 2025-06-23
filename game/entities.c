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
        "info_player_start2",
        "trigger_teleport",
        "trigger_changelevel",
        NULL
    };
    void (*Constructors[])(Entity* self) = {
        InfoPlayerStart,
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
    if (StrEq(globals->map, "start")) {
        if (globals->status == 0 && StrEq(self->className, "info_player_start2")) {
            return;
        } else if (globals->status > 0 && !StrEq(self->className, "info_player_start2")) {
            return;
        }
    }
    EntPtr ent = i.Spawn(self);
    SetVec3(ent->mins, -16, -16, -24);
    SetVec3(ent->maxs,  16,  16,  32);
    ent->eyePos = 22;
    ++ent->origin[2];
    i.SpawnPlayer(ent);
}

void    TouchTeleport(Entity* self, Entity* other)
{
    const char* target = i.EntityValueStr(self, "target");
    if (target != NULL && i.EntityValueStr(self, "targetname") != NULL)
    {
        return;
    }
    EntPtr targetEnt = i.SearchEntity("info_teleport_destination", "targetname", target);
    if (targetEnt != NULL) {
        Vec3 origin;
        CopyVec3(origin, targetEnt->origin);
        float angle = targetEnt->angle;
        origin[2] -= other->mins[2];
        origin[2]++;
        i.SetPlayerPosAngle(origin, angle);
    }
}
void    TriggerTeleport(Entity* self)
{
    EntPtr ent = i.Spawn(self);
    ent->Touch = TouchTeleport;
}

void    TouchChangelevel(Entity* self, Entity* other)
{
    UNUSED(other);
    if (globals->status == 0) {
        globals->status = 1;
    }
    const char* map = i.EntityValueStr(self, "map");
    char path[1024] = "";
    strcat(path, "maps/");
    strcat(path, map);
    strcat(path, ".bsp");
    i.PostCommand(1, path, 0, 0);
}
void    TriggerChangelevel(Entity* self)
{
    EntPtr ent = i.Spawn(self);
    ent->Touch = TouchChangelevel;
}
