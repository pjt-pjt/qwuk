#pragma once

#include "entity.h"
#include "glm/glm.hpp"


class BSP;
class Actor;


class PlayerMove
{
public: 
    PlayerMove(BSP& bsp);

    void    Move(Actor& player, const glm::vec3& velocityBase, float elapsed);
    const glm::vec3& Origin() const
    {
        return origin;
    }

private:
    void    AirMove(const glm::vec3& wishVelocity);
    void    FlyMove();
    void    GroundMove();
    enum AccelerateMode { OnGround, InAir };
    void    Accelerate(AccelerateMode mode, const glm::vec3& wishDir, float wishSpeed, float accel);
    void    Friction();
    void    ClipVelocity (const glm::vec3& in, const glm::vec3& normal, glm::vec3& out, float overbounce);
    void    CategorizePosition();

private:
    BSP&        bsp;
    glm::vec3   origin;
    float       frameTime;

    glm::vec3   velocity;
    int         onground = 0;
    int		    numTouch;
    static constexpr int MAX_TOUCHENTS = 32;
	EntPtr      touchEnts[MAX_TOUCHENTS];
};
