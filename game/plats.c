#include "tools.h"
#include "game.h"
#include "entities.h"


void    UsePlat(EntPtr self, EntPtr other);
void    TouchPlat(EntPtr self, EntPtr other);

void    RunPlat(EntPtr self)
{
    Vec3Ref target;
    Vec3    direction;
    Vec3Set(direction, 0, 0, -1);

    if (self->f->platStatus == PLAT_TOP) {
        target = self->f->pos2;
        //Vec3Copy(direction, /* self->f-> */direction);
    } else {
        target = self->f->pos1;
        Vec3Sub(direction, Origin_Vec3, /* self->f-> */direction);
    }
    Vec3    velocity;
    Vec3Mul(velocity, direction, self->f->speed * globals->frameTime);
    if (Vec3LengthSq(velocity) >= Vec3DistanceSq(target, self->origin)) {
        i.SetOrigin(self, target);
        self->f->platStatus = !self->f->platStatus;
        self->sleep = -1;
        self->Think = NULL;
        if (self->f->platStatus == PLAT_TOP) {
            self->sleep = 3;
            self->Think = RunPlat;
        } else {
            self->Touch = TouchPlat;
        }
        return;
    }
    Vec3    origin;
    Vec3Add(origin, self->origin, velocity);
    i.SetOrigin(self, origin);
    self->sleep = 0;
}

void    BlockedPlat(EntPtr self, EntPtr by)
{
    UNUSED(by);
    self->f->platStatus = !self->f->platStatus;
    self->sleep = .5;
    self->Think = RunPlat;
}


void    UsePlat(EntPtr self, EntPtr other)
{
    UNUSED(other);
    self->sleep = 1;
    self->Think = RunPlat;
}

void    TouchPlat(EntPtr self, EntPtr other)
{
    UNUSED(other);
    //TODO If player on plat
    self->sleep = .2;
    self->Touch = NULL;
    self->Think = RunPlat;
}

void    FuncPlat(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->f = NewFields();

    Vec3Sub(self->f->size, self->maxs, self->mins);
    float speed;
    if (!i.EntityValueFloat(self, "speed", &speed)) {
        speed = 150;
    }
    self->f->speed = speed;
    float height;
    if (!i.EntityValueFloat(self, "height", &height)) {
        height = self->f->size[2] - 8;
    }

    Vec3Copy(self->f->pos1, self->origin);
    Vec3Copy(self->f->pos2, self->origin);
    self->f->pos2[2] = self->f->pos2[2] - height;

    if (i.EntityValueStr(self, "targetname") != NULL) {
        self->f->platStatus = PLAT_TOP;
        self->Use = UsePlat;
    } else {
        i.SetOrigin(self, self->f->pos2);
        self->f->platStatus = PLAT_BOTTOM;
        self->Touch = TouchPlat;
    }
    self->Blocked = BlockedPlat;
}
