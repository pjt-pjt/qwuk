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
    bool    PushMove(EntPtr entity, Vec3 move);

private:
    bool    Collide(EntPtr entity1, EntPtr entity2);

private:
    BSP&        bsp;
    Quake&      quake;
    EntPtr      pushedEntities[128];
    int         numPushed = 0;
};
