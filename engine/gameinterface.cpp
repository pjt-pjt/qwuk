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

const char*   GameInterface::EntityClass(int entity)
{
    if (entity < 0 || entity >= int(game->bsp.Entities().size())) {
        return nullptr;
    }
    return game->bsp.Entities()[entity].className.c_str();
}

const char*   GameInterface::EntityValueStr(int entity, const char* key)
{
    if (entity < 0 || entity >= int(game->bsp.Entities().size())) {
        return nullptr;
    }
    const Entity&   ent = game->bsp.Entities()[entity];
    Entity::Value   val = ent.GetValue(key);
    if (val.empty()) {
        return nullptr;
    }
    return val.c_str();
}

int     GameInterface::EntityValueFloat(int entity, const char* key, float* value)
{
    if (entity < 0 || entity >= int(game->bsp.Entities().size())) {
        return 0;
    }

    const Entity& ent = game->bsp.Entities()[entity];
    std::string   skey = key;
    if (skey == "angle") {
        *value = ent.angle;
        return 1;
    }

    Entity::Value   val = ent.GetValue(skey);
    if (val.empty()) {
        return 0;
    }
    *value = std::stof(val);
    return 1;
}

int     GameInterface::EntityValueVec3(int entity, const char* key, float* value)
{
    if (entity < 0 || entity >= int(game->bsp.Entities().size())) {
        return 0;
    }

    const Entity& ent = game->bsp.Entities()[entity];
    std::string   skey = key;
    if (skey == "origin") {
        memcpy_s(value, 3 * sizeof(float), glm::value_ptr(ent.origin), 3 * sizeof(float));
        return 1;
    }

    Entity::Value   val = ent.GetValue(skey);
    if (val.empty()) {
        return 0;
    }
    std::sscanf(val.c_str(), "%f %f %f", &value[0], &value[1], &value[2]);
    return 1;
}

int     GameInterface::SearchEntity(const char* className, const char* key, const char* value)
{
    for (int ei = 0; ei < int(game->bsp.Entities().size()); ++ei) {
        const auto& entity = game->bsp.Entities()[ei];
        if (entity.className == className) {
            Entity::Value   val = entity.GetValue(key);
            if (!val.empty() && val == value) {
                return ei;
            }
        }
    }
    return -1;
}

void    GameInterface::SpawnPlayer(int entity)
{
    const Entity& e = game->bsp.Entities()[entity];
    game->quake.player.Init(e);
}

void    GameInterface::TeleportPlayer(const float* origin, float angle)
{
    glm::vec3   pos(origin[0], origin[1], origin[2]);
    pos.z -= game->quake.player.mins.z;
    game->quake.player.SetPosition(pos);
    game->quake.player.SetYaw(angle);
}