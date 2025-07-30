#pragma once

#include "entity.h"
#include "bsp.h"
#include "glm/glm.hpp"


class Actor;


class PlayerMove
{
public: 
    PlayerMove(BSP& bsp, Actor& player);

    void    Use();
    void    Move(const glm::vec3& velocityBase, float elapsed);
    void    Fly(const glm::vec3& velocityBase, float elapsed);
    void    SetVelocity(const glm::vec3& velocityBase);
    void    SetKeys(bool jumpKeyDown, bool useKeyDown);
    const glm::vec3& Origin() const
    {
        return origin;
    }
    const glm::vec3& Velocity() const
    {
        return velocity;
    }

private:
    void    WaterMove(const glm::vec3& wishVelocity);
    void    AirMove(const glm::vec3& wishVelocity);
    void    FlyMove();
    void    GroundMove();
    void    Jump();
    void    CheckWaterJump();

    enum AccelerateMode { OnGround, InAir };
    void    Accelerate(AccelerateMode mode, const glm::vec3& wishDir, float wishSpeed, float accel);
    void    Friction();
    void    ClipVelocity (const glm::vec3& in, const glm::vec3& normal, glm::vec3& out, float overbounce);
    void    CategorizePosition();
    void    NudgePosition();

    Trace   MovePlayer(const glm::vec3& start, const glm::vec3& end);
    bool    TestPlayerPosition(const glm::vec3& pos);

    void    TouchEnt(EntPtr entity);

private:
    BSP&        bsp;
    Actor&      player;
    glm::vec3   origin;
    float       frameTime;

    glm::vec3   velocity;
    bool        jumpKeyDown = false;
    bool        useKeyDown = false;
    bool        jumpKey = false;
    bool        useKey = false;
public:
    EntPtr      onground = nullptr;
    EntPtr      useEnt = nullptr;
    int         waterlevel = 0;
    float       waterjumptime = 0;
    LeafType    watertype = EMPTY;
    //TODO
    glm::vec3   baseVelocity;
};
