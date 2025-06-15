#include "gameinterface.h"
#include "quake.h"
#include "game.h"
#include "glm/gtc/type_ptr.hpp"


GameInterface* game = nullptr;


GameInterface::GameInterface(Quake& quake) :
    quake(quake)
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
    functions->SetPlayer = SetPlayer;
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
    if (entity < 0 || entity >= int(game->quake.bsp.Entities().size())) {
        return nullptr;
    }
    return game->quake.bsp.Entities()[entity].className.c_str();
}

const char*   GameInterface::EntityValueStr(int entity, const char* key)
{
    if (entity < 0 || entity >= int(game->quake.bsp.Entities().size())) {
        return nullptr;
    }
    const Entity& ent = game->quake.bsp.Entities()[entity];
    Entity::Result  res = ent.GetValue(key);
    if (!res) {
        return nullptr;
    }
    return (*res)->c_str();
}

int     GameInterface::EntityValueFloat(int entity, const char* key, float* value)
{
    if (entity < 0 || entity >= int(game->quake.bsp.Entities().size())) {
        return 0;
    }

    const Entity& ent = game->quake.bsp.Entities()[entity];
    std::string   skey = key;
    if (skey == "angle") {
        *value = ent.angle;
        return 1;
    }

    Entity::Result  res = ent.GetValue(skey);
    if (!res) {
        return 0;
    }
    *value = std::stof(**res);
    return 1;
}

int     GameInterface::EntityValueVec3(int entity, const char* key, float* value)
{
    if (entity < 0 || entity >= int(game->quake.bsp.Entities().size())) {
        return 0;
    }

    const Entity& ent = game->quake.bsp.Entities()[entity];
    std::string   skey = key;
    if (skey == "origin") {
        memcpy_s(value, 3 * sizeof(float), glm::value_ptr(ent.origin), 3 * sizeof(float));
        return 1;
    }

    Entity::Result  res = ent.GetValue(skey);
    if (!res) {
        return 0;
    }
    std::sscanf((*res)->c_str(), "%f %f %f", &value[0], &value[1], &value[2]);
    return 1;
}

int     GameInterface::SearchEntity(const char* className, const char* key, const char* value)
{
    for (int ei = 0; ei < int(game->quake.bsp.Entities().size()); ++ei) {
        const auto& entity = game->quake.bsp.Entities()[ei];
        if (entity.className == className) {
            Entity::Result  result = entity.GetValue(key);
            if (result && **result == value) {
                return ei;
            }
        }
    }
    return -1;
}

void    GameInterface::SetPlayer(const float* origin, float angle)
{
    glm::vec3   pos(origin[0], origin[1], origin[2]);
    game->quake.player.SetPosition(pos);
    game->quake.player.SetYaw(angle);
}

void    GameInterface::TeleportPlayer(const float* origin, float angle)
{
    glm::vec3   pos(origin[0], origin[1], origin[2]);
    pos.z -= game->quake.player.mins.z;
    game->quake.player.SetPosition(pos);
    game->quake.player.SetYaw(angle);
}