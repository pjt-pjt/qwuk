#include "entities.h"
#include "tools.h"
#include "game.h"
#include <stdio.h>
#include <math.h>


Fields  fieldList[2048];
int     numFields;


void    Spawn(Entity* ent);
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
        "worldspawn",
        "info_player_start",
        "info_player_start2",
        "trigger_teleport",
        "trigger_changelevel",
        "trigger_onlyregistered",
        "func_door",
        NULL
    };
    void (*Constructors[])(Entity* self) = {
        Spawn,
        InfoPlayerStart,
        InfoPlayerStart,
        TriggerTeleport,
        TriggerChangelevel,
        NULL,
        FuncDoor
    };

    for (int cl = 0; Classes[cl] != NULL; ++cl) {
        if (StrEq(entity->className, Classes[cl]) && Constructors[cl] != NULL) {
            Constructors[cl](entity);
            return;
        }
    }

    //Hack construct all necessary
    if (StrPrefix(entity->className, "info_") ||
        StrPrefix(entity->className, "trigger_") ||
        StrPrefix(entity->className, "func_"))
    {
        i.Spawn(entity);
    }
}

int     Touching(EntPtr ent1, EntPtr ent2)
{
    if (ent1->maxs[0] < ent2->mins[0]) {
        return 0;
    }
    if (ent1->mins[0] > ent2->maxs[0]) {
        return 0;
    }

    if (ent1->maxs[1] < ent2->mins[1]) {
        return 0;
    }
    if (ent1->mins[1] > ent2->maxs[1]) {
        return 0;
    }

    if (ent1->maxs[2] < ent2->mins[2]) {
        return 0;
    }
    if (ent1->mins[2] > ent2->maxs[2]) {
        return 0;
    }
    return 1;
}


void    Spawn(Entity* ent)
{
    i.Spawn(ent);
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
    EntPtr targetEnt = i.SearchEntity(NULL, "info_teleport_destination", "targetname", target);
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


void    UseDoor(Entity* self, Entity* other)
{
    UNUSED(other);
    EntPtr  ent = self->owner;
    while (ent != NULL) {
        i.SetOrigin(ent, ent->f->pos2);
        ent->Use = NULL;
        ent = ent->link;
    }
}
void    LinkDoors(Entity* self)
{
    if (self->owner != NULL) {
        // Already linked
        return;
    }
    EntPtr prev = NULL;
    EntPtr ent = i.SearchEntity(NULL, self->className, NULL, NULL);
    while (ent != NULL) {
        if (ent != self && Touching(self, ent)) {
            ent->owner = self;
            ent->link = prev;
            ent->Use = UseDoor;
            ent->wait = -1;
            ent->Think = NULL;
            prev = ent;
        }
        ent = i.SearchEntity(ent, self->className, NULL, NULL);
    }
    self->owner = self;
    self->link = prev;
    self->Use = UseDoor;
    self->wait = -1;
    self->Think = NULL;
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
    float lip = 8;
    if (ent->flags == 1) {
        lip = 0; //TODO Hack for "elevator" doors that start "open"
    }
    float dot = fabs(Vec3Dot(self->f->direction, self->f->size)) - lip;
    Vec3AddMul(self->f->pos2, self->f->pos1, self->f->direction, dot);
    self->Think = LinkDoors;
    self->wait = .1;
}
