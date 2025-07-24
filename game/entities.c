#include "entities.h"
#include "tools.h"
#include "game.h"
#include <stdio.h>
#include <math.h>


Fields  fieldList[2048];
int     numFields;


void    Spawn(EntPtr ent);
void    InfoPlayerStart(EntPtr ent);
void    TriggerTeleport(EntPtr ent);
void    TriggerChangelevel(EntPtr ent);
void    FuncDoor(EntPtr ent);
void    FuncButton(EntPtr ent);


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


void    Construct(EntPtr entity)
{
    const char* Classes[] = {
        "worldspawn",
        "info_player_start",
        "info_player_start2",
        "trigger_teleport",
        "trigger_changelevel",
        "trigger_onlyregistered",
        "func_door",
        "func_button",
        NULL
    };
    void (*Constructors[])(EntPtr self) = {
        Spawn,
        InfoPlayerStart,
        InfoPlayerStart,
        TriggerTeleport,
        TriggerChangelevel,
        NULL,
        FuncDoor,
        FuncButton
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


void    Spawn(EntPtr ent)
{
    i.Spawn(ent);
}

void    InfoPlayerStart(EntPtr ent)
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

void    TouchTeleport(EntPtr self, EntPtr other)
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
void    TriggerTeleport(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchTeleport;
}

void    TouchChangelevel(EntPtr self, EntPtr other)
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
void    TriggerChangelevel(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchChangelevel;
}


void    UseDoor(EntPtr self, EntPtr other);
void    RunDoor(EntPtr self)
{
    Vec3Ref target;
    Vec3    direction;
    if (self->f->doorStatus == DOOR_CLOSED) {
        target = self->f->pos2;
        Vec3Copy(direction, self->f->direction);
    } else {
        target = self->f->pos1;
        Vec3Sub(direction, Origin_Vec3, self->f->direction);
    }
    Vec3    velocity;
    Vec3Mul(velocity, direction, self->f->speed * globals->frameTime);
    if (Vec3LengthSq(velocity) >= Vec3DistanceSq(target, self->origin)) {
        i.SetOrigin(self, target);
        self->f->doorStatus = !self->f->doorStatus;
        self->sleep = -1;
        self->Think = NULL;
        if (self->f->doorStatus == DOOR_OPEN) {
            if (self->f->wait != -1) {
                self->sleep = self->f->wait;
                self->Think = RunDoor;
            }
        } else {
            if (self->f->wait != -1) {
                self->Use = UseDoor;
            }
        }
        return;
    }
    Vec3    origin;
    Vec3Add(origin, self->origin, velocity);
    i.SetOrigin(self, origin);
    self->sleep = 0;
}
void    UseDoor(EntPtr self, EntPtr other)
{
    UNUSED(other);
    EntPtr  ent = self->owner;
    while (ent != NULL) {
        ent->Use = NULL;
        ent->sleep = 0;
        ent->Think = RunDoor;
        ent = ent->link;
    }
}
void    BlockedDoor(EntPtr self, EntPtr by)
{
    UNUSED(by);
    if (self->f->doorStatus == DOOR_CLOSED) {
        return;
    }
    self->f->doorStatus = !self->f->doorStatus;
    self->sleep = .5;
    self->Think = RunDoor;
}
void    LinkDoors(EntPtr self)
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
            ent->sleep = -1;
            ent->Think = NULL;
            prev = ent;
        }
        ent = i.SearchEntity(ent, self->className, NULL, NULL);
    }
    self->owner = self;
    self->link = prev;
    self->Use = UseDoor;
    self->sleep = -1;
    self->Think = NULL;
}
void    FuncDoor(EntPtr ent)
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
        lip = 0; //TODO Hack for "elevator" or "slab" doors that start "open"
    }
    float dot = fabs(Vec3Dot(self->f->direction, self->f->size)) - lip;
    Vec3AddMul(self->f->pos2, self->f->pos1, self->f->direction, dot);
    if (ent->flags == 1) {
        i.SetOrigin(self, self->f->pos2);
        self->f->doorStatus = DOOR_OPEN;
    } else {
        self->f->doorStatus = DOOR_CLOSED;
    }
    self->Think = LinkDoors;
    self->sleep = .1;
    float speed;
    if (!i.EntityValueFloat(self, "speed", &speed)) {
        speed = 100;
    }
    self->f->speed = speed;
    float wait;
    if (!i.EntityValueFloat(self, "wait", &wait)) {
        wait = 3;
    }
    self->f->wait = wait;
    self->Blocked = BlockedDoor;
}

void    UseButton(EntPtr self, EntPtr other);
void    RunButton(EntPtr self)
{
    Vec3Ref target;
    Vec3    direction;
    if (self->f->doorStatus == BUTTON_WAIT) {
        target = self->f->pos2;
        Vec3Copy(direction, self->f->direction);
    } else {
        target = self->f->pos1;
        Vec3Sub(direction, Origin_Vec3, self->f->direction);
    }
    Vec3    velocity;
    Vec3Mul(velocity, direction, self->f->speed * globals->frameTime);
    if (Vec3LengthSq(velocity) >= Vec3DistanceSq(target, self->origin)) {
        i.SetOrigin(self, target);
        self->f->buttonStatus = !self->f->buttonStatus;
        if (self->f->buttonStatus == BUTTON_PRESSED) {
            if (self->f->wait != -1) {
                self->sleep = self->f->wait;
                self->Think = RunButton;
            } else {
                self->Think = NULL;
            }
            // Use targets
            const char* target = i.EntityValueStr(self, "target");
            if (target != NULL) {
                EntPtr  ent = i.SearchEntity(NULL, NULL, "targetname", target);
                while (ent != NULL) {
                    if (ent->Use != NULL) {
                        ent->Use(ent, self);
                    }
                    ent = i.SearchEntity(ent, NULL, "targetname", target);
                }
            }
        } else {
            self->Think = NULL;
            if (self->f->wait != -1) {
                self->Use = UseButton;
            }
        }
        return;
    }
    Vec3    origin;
    Vec3Add(origin, self->origin, velocity);
    i.SetOrigin(self, origin);
    self->sleep = 0;
}
void    UseButton(EntPtr self, EntPtr other)
{
    UNUSED(other);
    self->Use = NULL;
    self->sleep = 0;
    self->Think = RunButton;
}
void    FuncButton(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->f = NewFields();
    self->f->buttonStatus = BUTTON_WAIT;

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
    float lip;
    if (!i.EntityValueFloat(self, "lip", &lip)) {
        lip = 4;
    }
    float dot = fabs(Vec3Dot(self->f->direction, self->f->size)) - lip;
    Vec3AddMul(self->f->pos2, self->f->pos1, self->f->direction, dot);
    float speed;
    if (!i.EntityValueFloat(self, "speed", &speed)) {
        speed = 40;
    }
    self->f->speed = speed;
    float wait;
    if (!i.EntityValueFloat(self, "wait", &wait)) {
        wait = 1;
    }
    self->f->wait = wait;
    self->Use = UseButton;
}