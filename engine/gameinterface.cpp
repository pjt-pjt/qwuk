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
    interface->SetPos = SetPos;
    interface->SetAngle = SetAngle;
    interface->SetPlayerPos = SetPlayerPos;
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

EntPtr  GameInterface::SearchEntity(const char* className, const char* key, const char* value)
{
    for (auto& entity : game->bsp.actEntities /* entities.entities */) {
        if (StrEq(entity.className, className)) {
            const char*   val = EntityValueStr(&entity, key);
            if (val != nullptr && StrEq(val, value)) {
                return &entity;
            }
        }
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

void    GameInterface::SetPos(EntPtr entity, const Vec3 origin)
{
    if (entity == NULL) {
        return;
    }
    Vec3Copy(entity->origin, origin);
}

void    GameInterface::SetAngle(EntPtr entity, float angle)
{
    if (entity == NULL) {
        return;
    }
    entity->angle = angle;
}

void    GameInterface::SetPlayerPos(const Vec3 origin)
{
    game->quake.player.SetPosition({origin[0], origin[1], origin[2]});
}

void    GameInterface::SetPlayerAngle(float angle)
{
    game->quake.player.SetYaw(angle);
}
