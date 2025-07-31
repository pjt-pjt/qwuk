#pragma once

#include "entity.h"


class BSP;


class Physics
{
public:
    Physics(BSP& bsp);
    

    void    PushMove(EntPtr entity, Vec3 move);

private:
    BSP&        bsp;
};
