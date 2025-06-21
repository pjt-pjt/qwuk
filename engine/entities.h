#pragma once

#include <memory>
#include <cstdint>
#include <vector>
#include "entity.h"


class Entities
{
public:
    bool    Init(const char* entities, uint32_t entitiesSize);
    void    Destroy();

    static const char* EntityValueStr(const Entity& entity, const char* key);
    static int         EntityValueFloat(const Entity& entity, const char* key, float* value);
    static int         EntityValueVec3(const Entity& entity, const char* key, Vec3 value);

private:
    std::unique_ptr<char>   strings;
    std::vector<Edict>      edicts;
public:
    std::vector<Entity>     entities;
};

