#pragma once

#include "glm/glm.hpp"


class BSP;
class Actor;


class PlayerMove
{
public: 
    PlayerMove(BSP& bsp);

    void    Move(Actor& player, const glm::vec3& velocityBase, float elapsed);

private:
    void    AirMove(const glm::vec3& wishVelocity);
    enum AccelerateMode { OnGround, InAir };
    void    Accelerate(AccelerateMode mode, const glm::vec3& wishDir, float wishSpeed, float accel);
    void    Friction();

private:
    BSP&        bsp;
    glm::vec3   velocity;
    int         onground = 0;
    float       elapsed;
};
