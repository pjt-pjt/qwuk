#include "gameinterface.h"
#include "quake.h"
#include "game.h"
#include "glm/gtc/type_ptr.hpp"


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
    interface->PostCommand = AddCommand;
    interface->EnumerateEntites = EnumerateEntites;
    interface->EntityClass = EntityClass;
    interface->EntityValueStr = EntityValueStr;
    interface->EntityValueFloat = EntityValueFloat;
    interface->EntityValueVec3 = EntityValueVec3;
    interface->SearchEntity = SearchEntity;
    interface->SpawnPlayer = SpawnPlayer;
    interface->TeleportPlayer = TeleportPlayer;
}

void    GameInterface::AddCommand(int command, const char* strParam1, float /* fltParam1 */, int /* intParam1 */)
{
    if (game == nullptr) {
        return;
    }
    game->quake.AddCommand({Command::Cmd(command), 2, strParam1});
}

EntPtr  GameInterface::EnumerateEntites(EntPtr from)
{
    const Entity*  begin = &game->bsp.entities.entities[0];
    if (from == nullptr) {
        return begin;
    }
    const Entity*  end = begin + game->bsp.entities.entities.size();
    const Entity*  efrom = reinterpret_cast<const Entity*>(from);
    if (efrom >= begin && efrom < end) {
        return ++efrom;
    }
    return nullptr;
}

const char*   GameInterface::EntityClass(EntPtr entity)
{
    if (entity == nullptr) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    return ent.className;
}

const char*   GameInterface::EntityValueStr(EntPtr entity, const char* key)
{
    if (entity == nullptr) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    return Entities::EntityValueStr(ent, key);
}

int     GameInterface::EntityValueFloat(EntPtr entity, const char* key, float* value)
{
    if (entity == nullptr) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    if (Equals(key, "angle")) {
        *value = ent.angle;
        return 1;
    }
    return Entities::EntityValueFloat(ent, key, value);
}

int     GameInterface::EntityValueVec3(EntPtr entity, const char* key, float* value)
{
    if (entity == nullptr) {
        return 0;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    if (Equals(key, "origin")) {
        memcpy_s(value, 3 * sizeof(float), ent.origin, 3 * sizeof(float));
        return 1;
    }
    return Entities::EntityValueVec3(ent, key, value);
}

EntPtr  GameInterface::SearchEntity(const char* className, const char* key, const char* value)
{
    for (const auto& entity : game->bsp.entities.entities) {
        if (Equals(entity.className, className)) {
            const char*   val = EntityValueStr(&entity, key);
            if (val != nullptr && Equals(val, value)) {
                return &entity;
            }
        }
    }
    return NULL;
}

void    GameInterface::SpawnPlayer(EntPtr entity)
{
    if (entity == nullptr) {
        return;
    }
    const Entity& ent = *reinterpret_cast<const Entity*>(entity);
    game->quake.player.Init(ent);
}

void    GameInterface::TeleportPlayer(const float* origin, float angle)
{
    glm::vec3   pos(origin[0], origin[1], origin[2]);
    pos.z -= game->quake.player.mins.z;
    game->quake.player.SetPosition(pos);
    game->quake.player.SetYaw(angle);
}