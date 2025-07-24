#include "tools.h"
#include "game.h"
#include "entities.h"


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
