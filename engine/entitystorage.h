#pragma once

#include "entity.h"
#include <memory>
#include <cstdint>
#include <vector>


class EntityStorage
{
public:
    bool    Init(const char* entities, uint32_t entitiesSize);

private:
    std::unique_ptr<char>   strings;
    std::vector<Edict>      edicts;
    std::vector<Entity_>     entities;
};
