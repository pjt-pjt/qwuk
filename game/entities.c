#include "entities.h"
#include "tools.h"
#include "game.h"
#include <stdio.h>
#include <math.h>


Fields  fieldList[2048];
int     numFields;


void    InfoPlayerStart(Entity* ent);
void    TriggerTeleport(Entity* ent);
void    TriggerChangelevel(Entity* ent);
void    FuncDoor(Entity* ent);


void    ResetFields()
{
    numFields = 0;
}

Fields* NewFields()
{
    Fields* fields = &fieldList[numFields++];
    memset(fields, 0, sizeof(Fields));
    return fields;
}


void    Construct(Entity* entity)
{
    const char* Classes[] = {
        "info_player_start",
        "info_player_start2",
        "trigger_teleport",
        "trigger_changelevel",
        "func_door",
        NULL
    };
    void (*Constructors[])(Entity* self) = {
        InfoPlayerStart,
        InfoPlayerStart,
        TriggerTeleport,
        TriggerChangelevel,
        FuncDoor
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


void    InfoPlayerStart(Entity* ent)
{
    if (StrEq(globals->map, "start")) {
        if (globals->status == 0 && StrEq(ent->className, "info_player_start2")) {
            return;
        } else if (globals->status > 0 && !StrEq(ent->className, "info_player_start2")) {
            return;
        }
    }
    EntPtr self = i.Spawn(ent);
    Vec3Set(self->mins, -16, -16, -24);
    Vec3Set(self->maxs,  16,  16,  32);
    self->eyePos = 22;
    ++self->origin[2];
    i.SpawnPlayer(self);
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
        Vec3Copy(origin, targetEnt->origin);
        float angle = targetEnt->angle;
        origin[2] -= other->mins[2];
        ++origin[2];
        i.SetPlayerOrigin(origin);
        i.SetPlayerAngle(angle);
    }
}
void    TriggerTeleport(Entity* ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchTeleport;
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
void    TriggerChangelevel(Entity* ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchChangelevel;
}

void    FuncDoor(Entity* ent)
{
    EntPtr self = i.Spawn(ent);
    self->f = NewFields();
    Vec3Sub(self->f->size, self->maxs, self->mins);
    Vec3Copy(self->f->pos1, self->origin);
    if (self->angle == 0) {
        self->f->direction[0] =  1;
    } else if (self->angle == 180) {
        self->f->direction[0] = -1;
    } else if (self->angle == 90) {
        self->f->direction[1] =  1;
    } else if (self->angle == 270) {
        self->f->direction[1] = -1;
    } else if (self->angle == -1) {
        self->f->direction[2] =  1;
    } else if (self->angle == -2) {
        self->f->direction[2] = -1;
    }
    float dot = fabs(Vec3Dot(self->f->direction, self->f->size)) - 8/*lip*/;
    Vec3AddMul(self->f->pos2, self->f->pos1, self->f->direction, dot);
    //i.SetOrigin(self, self->f->pos2);
}