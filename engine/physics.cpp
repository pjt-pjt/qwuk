#include "physics.h"
#include "tools.h"
#include "bsp.h"
#include "client.h"


void    Physics::NextFrame(float /* frameTime */)
{
    numPushed = 0;
}

bool    Physics::Move(EntPtr entity, const Vec3 move)
{
    if (quake.playerMove.onground == entity) {
        return TrainMove(entity, move);
    } else /* if (movetype == PUSH_MOVE) */ {
        return PushMove(entity, move);
    }
}

bool    Physics::TrainMove(EntPtr entity, const Vec3 origin)
{
    Vec3        prevOrigin;
    Vec3Copy(prevOrigin, entity->origin);
    glm::vec3   vPrevOrigin = {prevOrigin[0], prevOrigin[1], prevOrigin[2]};
    glm::vec3   vOrigin = {origin[0], origin[1], origin[2]};
    glm::vec3   vMove = vOrigin - vPrevOrigin;
    bool        moved = true;

    Trace       tr = bsp.PlayerMove(quake.player.Position(), quake.player.Position() + vMove);
    bsp.SetOrigin(entity, vOrigin);
    if (tr.startSolid || tr.allSolid || tr.fraction < 1) {
        tr = bsp.PlayerMove(quake.player.Position(), quake.player.Position() + vMove);
        if (tr.allSolid || tr.fraction < 1) {
            moved = false;
        }
    }
    if (moved) {
        quake.player.SetPosition(quake.player.Position() + vMove);
    } else {
        bsp.SetOrigin(entity, vPrevOrigin);
    }
    return moved;
}

bool    Physics::PushMove(EntPtr entity, const Vec3 origin)
{
    //TODO Clip to whole move to avoid tunnel effect
    Vec3        prevOrigin;
    Vec3Copy(prevOrigin, entity->origin);
    glm::vec3   vPrevOrigin = {prevOrigin[0], prevOrigin[1], prevOrigin[2]};
    glm::vec3   vOrigin = {origin[0], origin[1], origin[2]};
    glm::vec3   vMove = vOrigin - vPrevOrigin;
    bool        moved = true;
    // for(all pushable entities) {
    if (Collide(entity, origin, quake.player.entity)) {
        Trace trace = bsp.PlayerMove(quake.player.Position(), quake.player.Position() + vMove);
        if (trace.allSolid || trace.fraction < 1) {
            moved = false;
        }
        pushedEntities[numPushed++] = entity;
        if (moved) {
            quake.player.SetPosition(quake.player.Position() + vMove);
        }
    }
    if (moved) {
        bsp.SetOrigin(entity, vOrigin);
    }
    // }
    return moved;
}

bool    Physics::Collide(EntPtr entity1, const Vec3 origin1, EntPtr entity2)
{   
    Vec3 mins1, maxs1, mins2, maxs2;
    Vec3Add(mins1, entity1->mins, origin1);
    Vec3Add(maxs1, entity1->maxs, origin1);
    Vec3Add(mins2, entity2->mins, entity2->origin);
    Vec3Add(maxs2, entity2->maxs, entity2->origin);
    return BoxesCollide(mins1, maxs1, mins2, maxs2);
}