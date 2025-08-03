#include "tools.h"
#include "game.h"
#include "entities.h"


//---- FuncDoor ----------------------------------------------------------------

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
        i.MoveTo(self, target);
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
    i.MoveTo(self, origin);
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
    if (self->f->wait == -1) {
        return;
    }
    self->f->doorStatus = !self->f->doorStatus;
    self->sleep = .5;
    self->Think = RunDoor;
}

int     CanLink(EntPtr self)
{
    const char* target = i.EntityValueStr(self, "targetname");
    if (target == NULL) {
        return 1;
    }
    EntPtr  targetter = i.SearchEntity(NULL, "trigger_onlyregistered", "target", target);
    if (targetter == NULL) {
        return 0;
    }
    targetter->Touch = NULL;
    return 1;
}

void    LinkDoors(EntPtr self)
{
    if (self->owner != NULL) {
        // Already linked
        return;
    }
    int     canLink = CanLink(self);
    EntPtr  prev = NULL;
    if (canLink) {
        EntPtr next = i.SearchEntity(NULL, self->className, NULL, NULL);
        while (next != NULL) {
            EntPtr ent = next;
            next = i.SearchEntity(next, self->className, NULL, NULL);
            if (ent == self) {
                continue;
            }
            if (!CanLink(ent)) {
                continue;
            }
            if (Touching(self, ent)) {
                ent->owner = self;
                ent->link = prev;
                ent->Use = UseDoor;
                ent->sleep = -1;
                ent->playerUse = 1;
                ent->Think = NULL;
                prev = ent;
            }
        }
    }
    self->owner = self;
    self->link = prev;
    self->Use = UseDoor;
    self->sleep = -1;
    self->playerUse = canLink;
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
    if ((ent->flags & 1) == 1) {
        lip = 0; //TODO Hack for "elevator" or "slab" doors that start "open"
    }
    float dot = fabs(Vec3Dot(self->f->direction, self->f->size)) - lip;
    Vec3AddMul(self->f->pos2, self->f->pos1, self->f->direction, dot);
    if ((ent->flags & 1) == 1) {
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


//---- FuncDoorSecret ----------------------------------------------------------
void    UseDoorSecret(EntPtr self, EntPtr other);

void    RunDoorSecret(EntPtr self)
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
        i.MoveTo(self, target);
        self->f->doorStatus = !self->f->doorStatus;
        self->sleep = -1;
        self->Think = NULL;
        if (self->f->doorStatus == DOOR_OPEN) {
            if ((self->flags & 1) != 1) {
                self->sleep = self->f->wait;
                self->Think = RunDoorSecret;
            }
        } else {
            //if (self->f->wait != -1) {
                self->Use = UseDoorSecret;
            //}
        }
        return;
    }
    Vec3    origin;
    Vec3Add(origin, self->origin, velocity);
    i.MoveTo(self, origin);
    self->sleep = 0;
}

void    UseDoorSecret(EntPtr self, EntPtr other)
{
    UNUSED(other);
    EntPtr  ent = self->owner;
    while (ent != NULL) {
        ent->Use = NULL;
        ent->sleep = 0;
        ent->Think = RunDoorSecret;
        ent = ent->link;
    }
}

void    FuncDoorSecret(EntPtr ent)
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

    float dot = fabs(Vec3Dot(self->f->direction, self->f->size));
    Vec3AddMul(self->f->pos2, self->f->pos1, self->f->direction, dot);
    // if ((ent->flags & 1) == 1) {
    //     i.SetOrigin(self, self->f->pos2);
    //     self->f->doorStatus = DOOR_OPEN;
    // } else {
        self->f->doorStatus = DOOR_CLOSED;
    // }
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
    //TODO self->Blocked = BlockedDoorSecret;
    if (i.EntityValueStr(self, "targetname") == NULL || (self->flags & 16) != 0) {
        self->playerUse = 1;
    }
    self->owner = self;
    self->Use = UseDoorSecret;
}
