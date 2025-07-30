#include "tools.h"
#include "game.h"
#include "entities.h"


//---- TriggerOnlyRegistered ---------------------------------------------------
void    TouchTriggerOnlyRegistered(EntPtr self, EntPtr other)
{
    UNUSED(other);
    UseTargets(self);
    self->Touch = NULL;
}

void    TriggerOnlyRegistered(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchTriggerOnlyRegistered;
}

//---- TouchTriggerOnce --------------------------------------------------------
void    TouchTriggerOnce(EntPtr self, EntPtr other)
{
    UNUSED(other);
    UseTargets(self);
    self->Touch = NULL;
}

void    TriggerOnce(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchTriggerOnce;
}

//---- TouchTriggerMultiple ----------------------------------------------------
void    TouchTriggerMultiple(EntPtr self, EntPtr other);

void    ResetTriggerMultiple(EntPtr self)
{
    if (self->playerUse) {
        self->Use = TouchTriggerMultiple;
    } else {
        self->Touch = TouchTriggerMultiple;
    }
    self->Think = NULL;
}

void    RunTriggerMultiple(EntPtr self)
{
    UseTargets(self);
    self->sleep = self->f->wait;
    self->Think = ResetTriggerMultiple;
}

void    TouchTriggerMultiple(EntPtr self, EntPtr other)
{
    UNUSED(other);
    self->Touch = NULL;
    self->Use = NULL;
    self->sleep = 0;
    self->Think = RunTriggerMultiple;
}

void    TriggerMultiple(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->f = NewFields();
    float health;
    int   playerUse = i.EntityValueFloat(self, "health", &health);
    self->playerUse = playerUse;
    if (playerUse) {
        self->Use = TouchTriggerMultiple;
    } else {
        self->Touch = TouchTriggerMultiple;
    }
    float wait;
    if (!i.EntityValueFloat(self, "wait", &wait)) {
        wait = .2;
    }
    self->f->wait = wait;
}

//---- TriggerCounter ----------------------------------------------------------
void    UseCounter(EntPtr self, EntPtr other)
{
    UNUSED(other);
    ++self->f->counter;
    if (self->f->counter == self->f->count) {
        // Use targets
        UseTargets(self);
    }
}

void    TriggerCounter(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->f = NewFields();
    self->f->counter = 0;
    if (!i.EntityValueInt(self, "count", &self->f->count)) {
        self->f->count = 2; // Default
    }

    self->Use = UseCounter;
}


//---- TriggerTeleport ---------------------------------------------------------
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


//---- TriggerChangelevel ------------------------------------------------------
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
