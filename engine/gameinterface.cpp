#include "gameinterface.h"
#include "quake.h"
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

    interface->EntityClass = EntityClass;
    interface->EntityValueStr = EntityValueStr;
    interface->EntityValueFloat = EntityValueFloat;
    interface->EntityValueVec3 = EntityValueVec3;

    interface->SetEntityFloat = SetEntityFloat;
    interface->SetEntityVec3 = SetEntityVec3;

    interface->PostCommand = PostCommand;
    interface->Spawn = Spawn;
    interface->SpawnPlayer = SpawnPlayer;
    interface->TeleportPlayer = TeleportPlayer;
}


EntPtr  GameInterface::EnumerateEntites(EntPtr from)
{
    Entity*  begin = &game->bsp.entities.entities[0];
    if (from == NULL) {
        return begin;
    }
    Entity*  end = begin + game->bsp.entities.entities.size();
    Entity*  efrom = reinterpret_cast<Entity*>(from);
    ++efrom;
    if (efrom > begin && efrom < end) {
        return efrom;
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


const char*   GameInterface::EntityClass(EntPtr entity)
{
    if (entity == NULL) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    return ent.className;
}

const char*   GameInterface::EntityValueStr(EntPtr entity, const char* key)
{
    if (entity == NULL) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    return Entities::EntityValueStr(ent, key);
}

int     GameInterface::EntityValueFloat(EntPtr entity, const char* key, float* value)
{
    if (entity == NULL) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    if (StrEq(key, "angle")) {
        *value = ent.angle;
        return 1;
    }
    return Entities::EntityValueFloat(ent, key, value);
}

int     GameInterface::EntityValueVec3(EntPtr entity, const char* key, float* value)
{
    if (entity == NULL) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    if (StrEq(key, "origin")) {
        memcpy_s(value, 3 * sizeof(float), ent.origin, 3 * sizeof(float));
        return 1;
    }
    return Entities::EntityValueVec3(ent, key, value);
}


void    GameInterface::SetEntityFloat(EntPtr entity, const char* member, float value)
{
    Entity& ent = *reinterpret_cast<Entity*>(entity);
    if (StrEq(member, "eyePos")) {
        ent.eyePos = value;
    }
}

void    GameInterface::SetEntityVec3(EntPtr entity, const char* member, Vec3 vec3)
{
    Entity& ent = *reinterpret_cast<Entity*>(entity);
    if (StrEq(member, "mins")) {
        CopyVec3(ent.mins, vec3);
    } else if (StrEq(member, "maxs")) {
        CopyVec3(ent.maxs, vec3);
    } else if (StrEq(member, "origin")) {
        CopyVec3(ent.origin, vec3);
    }
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
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    game->bsp.actEntities.push_back(ent);
    return &game->bsp.actEntities.back();
}

void    GameInterface::SpawnPlayer(EntPtr entity)
{
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    if (!StrPrefix(ent.className, "info_player_start")) {
        return;
    }
    game->quake.player.Init(ent);
}

void    GameInterface::TeleportPlayer(const float* origin, float angle)
{
    glm::vec3   pos(origin[0], origin[1], origin[2]);
    pos.z -= game->quake.player.mins.z;
    game->quake.player.SetPosition(pos);
    game->quake.player.SetYaw(angle);
}
