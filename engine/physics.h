#pragma once

#include "entity.h"


class BSP;
class Quake;


class Physics
{
public:
    Physics(BSP& bsp, Quake& quake) : bsp(bsp), quake(quake)
    {}

    void    NextFrame(float fameTime);
    bool    Move(EntPtr entity, const Vec3 origin);

private:
    bool    TrainMove(EntPtr entity, const Vec3 origin);
    bool    PushMove(EntPtr entity, const Vec3 origin);
    bool    Collide(EntPtr entity1, const Vec3 origin1, EntPtr entity2);

private:
    BSP&        bsp;
    Quake&      quake;
    EntPtr      pushedEntities[128];
    int         numPushed = 0;
};
