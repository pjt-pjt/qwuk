#pragma once

#include "entity.h"
#include <memory>
#include <cstdint>
#include <vector>


class Entities
{
public:
    bool    Init(const char* entities, uint32_t entitiesSize);
    void    Destroy();

    static const char* EntityValueStr(const Entity& entity, const char* key);

private:
    std::unique_ptr<char>   strings;
    std::vector<Edict>      edicts;
public:
    std::vector<Entity>     entities;
};


bool        Equals(const char* left, const char* right);
bool        StartsWith(const char* str, const char* subStr);
