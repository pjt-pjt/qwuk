#include "physics.h"
#include "tools.h"
#include "bsp.h"
#include "client.h"


void    Physics::NextFrame(float /* frameTime */)
{
    numPushed = 0;
}

bool    Physics::PushMove(EntPtr entity, Vec3 move)
{
    //TODO Clip to whole move to avoid tunnel effect
    glm::vec3   vmove = { move[0], move[1], move[2] };
    bool        moved = true;
    // for(all pushable entities) {
    if (Collide(entity, quake.player.entity)) {
        Trace trace = bsp.PlayerMove(quake.player.Position(), quake.player.Position() + vmove);
        if (trace.allSolid || trace.fraction < 1) {
            // if (entity->Blocked != NULL) {
            //     entity->Blocked(entity, quake.player.entity);
            // }
            moved = false;
        } else {
            quake.player.SetPosition(quake.player.Position() + vmove);
        }
        pushedEntities[numPushed++] = entity;
    }
    // }
    return moved;
}

bool    Physics::Collide(EntPtr entity1, EntPtr entity2)
{   
    Vec3 mins1, maxs1, mins2, maxs2;
    Vec3Add(mins1, entity1->mins, entity1->origin);
    Vec3Add(maxs1, entity1->maxs, entity1->origin);
    Vec3Add(mins2, entity2->mins, entity2->origin);
    Vec3Add(maxs2, entity2->maxs, entity2->origin);
    return BoxesCollide(mins1, maxs1, mins2, maxs2);
}