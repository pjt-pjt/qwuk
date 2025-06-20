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

void    GameInterface::Init(Functions* functions)
{
    functions->PostCommand = AddCommand;
    functions->EnumerateEntites = EnumerateEntites;
    functions->EntityClass = EntityClass;
    functions->EntityValueStr = EntityValueStr;
    functions->EntityValueFloat = EntityValueFloat;
    functions->EntityValueVec3 = EntityValueVec3;
    functions->SearchEntity = SearchEntity;
    functions->SpawnPlayer = SpawnPlayer;
    functions->TeleportPlayer = TeleportPlayer;
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
    return EntityStorage::EntityValueStr(ent, key);
}

int     GameInterface::EntityValueFloat(EntPtr entity, const char* key, float* value)
{
    if (Equals(key, "angle")) {
        if (entity == nullptr) {
            return 0;
        }
        const Entity& ent = *reinterpret_cast<const Entity*>(entity);
        *value = ent.angle;
        return 1;
    }

    const char*   val = EntityValueStr(entity, key);
    if (val == nullptr) {
        return 0;
    }
    *value = std::atof(val);
    return 1;
}

int     GameInterface::EntityValueVec3(EntPtr entity, const char* key, float* value)
{
    if (Equals(key, "angle")) {
        if (entity == nullptr) {
            return 0;
        }
        const Entity& ent = *reinterpret_cast<const Entity*>(entity);
        memcpy_s(value, 3 * sizeof(float), ent.origin, 3 * sizeof(float));
        return 1;
    }

    const char*   val = EntityValueStr(entity, key);
    if (val == nullptr) {
        return 0;
    }
    std::sscanf(val, "%f %f %f", &value[0], &value[1], &value[2]);
    return 1;
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