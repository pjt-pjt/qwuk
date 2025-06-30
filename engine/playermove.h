#pragma once


class BSP;


class PlayerMove
{
public:
    PlayerMove(BSP& bsp);

private:
    void    AirMove();

private:
    BSP&    bsp;
};
