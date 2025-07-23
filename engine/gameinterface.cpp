#include "gameinterface.h"
#include "client.h"
#include "game.h"
#include "glm/gtc/type_ptr.hpp"

#define TOOLS_IMPL
#include "tools.h"


GameInterface* game = nullptr;


GameInterface::GameInterface(Quake& quake) :
    quake(quake),
    bsp(quake.bsp)
{
    game = this;
}

GameInterface::~GameInterface()
{
    game = nullptr;
}

void    GameInterface::Init(Interface* interface)
{
    interface->EnumerateEntites = EnumerateEntites;
    interface->SearchEntity = SearchEntity;

    interface->EntityValueStr = EntityValueStr;
    interface->EntityValueFloat = EntityValueFloat;
    interface->EntityValueVec3 = EntityValueVec3;

    interface->PostCommand = PostCommand;
    interface->Spawn = Spawn;
    interface->SpawnPlayer = SpawnPlayer;
    interface->SetOrigin = SetOrigin;
    interface->SetAngle = SetAngle;
    interface->SetPlayerOrigin = SetPlayerOrigin;
    interface->SetPlayerAngle = SetPlayerAngle;
}


EntPtr  GameInterface::EnumerateEntites(EntPtr from)
{
    Entity*  begin = &game->bsp.entities.entities[0];
    if (from == NULL) {
        return begin;
    }
    Entity*  end = begin + game->bsp.entities.entities.size();
    ++from;
    if (from > begin && from < end) {
        return from;
    }
    return NULL;
}

EntPtr  GameInterface::SearchEntity(EntPtr from, const char* className, const char* key, const char* value)
{
    Entity*  begin = &game->bsp.actEntities[0];
    Entity*  ent = from;
    if (ent == NULL) {
        ent = begin;
    } else {
        ++ent;    
    }
    if (ent < begin) {
        return NULL;
    }
    Entity*  end = begin + game->bsp.actEntities.size();
    for (; ent < end; ++ent) {
        if (className != nullptr && !StrEq(ent->className, className)) {
            continue;
        }
        if (key != nullptr) {
            const char*   val = EntityValueStr(ent, key);
            if (val == nullptr) {
                continue;
            }
            if (value != nullptr && !StrEq(val, value)) {
                continue;
            }
        }
        return ent;
    }
    return NULL;
}


const char*   GameInterface::EntityValueStr(EntPtr entity, const char* key)
{
    if (entity == NULL) {
        return 0;
    }
    return Entities::EntityValueStr(*entity, key);
}

int     GameInterface::EntityValueFloat(EntPtr entity, const char* key, float* value)
{
    if (entity == NULL) {
        return 0;
    }
    return Entities::EntityValueFloat(*entity, key, value);
}

int     GameInterface::EntityValueVec3(EntPtr entity, const char* key, float* value)
{
    if (entity == NULL) {
        return 0;
    }
    return Entities::EntityValueVec3(*entity, key, value);
}


void    GameInterface::PostCommand(int command, const char* strParam1, float /* fltParam1 */, int /* intParam1 */)
{
    if (game == nullptr) {
        return;
    }
    game->quake.PostCommand({Command::Cmd(command), 2, strParam1});
}

EntPtr  GameInterface::Spawn(EntPtr entity)
{
    if (entity == NULL) {
        return NULL;
    }
    game->bsp.actEntities.push_back(*entity);
    return &game->bsp.actEntities.back();
}

void    GameInterface::SpawnPlayer(EntPtr entity)
{
    if (!StrPrefix(entity->className, "info_player_start")) {
        return;
    }
    game->quake.player.Init(*entity);
}

void    GameInterface::SetOrigin(EntPtr entity, const Vec3 origin)
{
    if (entity == NULL) {
        return;
    }
    glm::vec3   vPrevOrigin = {entity->origin[0], entity->origin[1], entity->origin[2]};
    glm::vec3   vOrigin = {origin[0], origin[1], origin[2]};
    bool        set = true;
    if (entity->model != -1) {
        // Check collision
        //TODO Do it properly in the future
        BSP::Model& model = game->bsp.models[entity->model];
        if (game->quake.playerMove.onground == entity) {
            // Move player too
            glm::vec3   offset = vOrigin - vPrevOrigin;
            Trace       tr = game->bsp.PlayerMove(game->quake.player.Position(), game->quake.player.Position() + offset);
            glm::mat4   mm(1);
            model.transform = glm::translate(mm, vOrigin);
            if (tr.startSolid || tr.allSolid || tr.fraction < 1) {
                tr = game->bsp.PlayerMove(game->quake.player.Position(), game->quake.player.Position() + offset);
                if (tr.startSolid || tr.allSolid || tr.fraction < 1) {
                    set = false;
                }
            }
            if (set) {
                game->quake.player.SetPosition(game->quake.player.Position() + offset);
            }
        } else {
            glm::mat4   mm(1);
            model.transform = glm::translate(mm, vOrigin);
            if (game->bsp.TracePoint(game->quake.player.Position()).content != EMPTY) {
                set = false;
            }
        }
        if (!set) {
            glm::mat4   mm(1);
            model.transform = glm::translate(mm, vPrevOrigin);
        }
    }
    if (set) {
        Vec3Copy(entity->prevOrigin, entity->origin);
        Vec3Copy(entity->origin, origin);
    }
}

void    GameInterface::SetAngle(EntPtr entity, float angle)
{
    if (entity == NULL) {
        return;
    }
    entity->angle = angle;
}

void    GameInterface::SetPlayerOrigin(const Vec3 origin)
{
    game->quake.player.SetPosition({origin[0], origin[1], origin[2]});
}

void    GameInterface::SetPlayerAngle(float angle)
{
    game->quake.SetYaw(angle);
}
