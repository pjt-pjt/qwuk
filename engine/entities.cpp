#include "entities.h"
#include "entity.h"
#include <vector>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "stringtools.h"


bool    Entities::Init(const char* entitiesStr, uint32_t entitiesSize)
{
    strings.reset(new char[entitiesSize]);
    edicts.reserve(2048);   //TODO arbitrary number
    edicts.clear();
    entities.clear();

    char*       buffPtr = strings.get();
    const char* inp = entitiesStr;
    const char* inpEnd = entitiesStr + entitiesSize;

    auto Eof = [&inp, &inpEnd]() -> bool {
        return inp >= inpEnd;
    };
    auto GetChar = [&inp, &Eof](char gc) -> bool {
        char c = *inp++;
        while ((c == ' ' || c == '\n') && !Eof()) {
            c = *inp++;
        }
        return !Eof() && (c == gc);
    };
    static constexpr uint32_t MAX_STRING_LENGTH = 255;
    auto GetString = [&inp, &GetChar, &Eof](char* string) -> bool {
        uint32_t    size = 0;
        if (!GetChar('\"')) {
            --inp;
            return false;
        }
        char c = *inp++;
        while (c != '\"' && !Eof() && size < MAX_STRING_LENGTH) {
            *string++ = c;
            c = *inp++;
            ++size;
        }
        if (Eof() && c != '\"') {
            --inp;
        }
        *string++ ='\0';
        return !Eof() && size < MAX_STRING_LENGTH;
    };
    auto StorePairStrings = [this, &buffPtr](Edict& edict) {
        char*       next = buffPtr;
        const char* ptr = edict.key;
        while(*ptr != '\0') {
            *buffPtr++ = *ptr++;
        }
        *buffPtr++ = '\0';
        edict.key = next;

        next = buffPtr;
        ptr = edict.value;
        while(*ptr != '\0') {
            *buffPtr++ = *ptr++;
        }
        *buffPtr++ = '\0';
        edict.value = next;
    };
    auto EdictsToEntity = [this] (const std::vector<Edict>& epairs) {
        Entity  entity;
        auto Search = [&epairs] (const char* key) -> int {
            for (uint32_t i = 0; i < epairs.size(); ++i) {
                if (StrEq(epairs[i].key, key)) {
                    return i;
                }
            }
            return -1;
        };
        int idx = Search("classname");
        if (idx == -1) {
            return;
        }
        entity.className = epairs[idx].value;
        idx = Search("origin");
        if (idx != -1) {
            sscanf(epairs[idx].value, "%f %f %f", &entity.origin[0], &entity.origin[1], &entity.origin[2]);
        }
        idx = Search("angle");
        if (idx != -1) {
            sscanf(epairs[idx].value, "%f", &entity.angle);
        }
        idx = Search("model");
        if (idx != -1) {
            sscanf(epairs[idx].value, "*%d", &entity.model);
        } else if (StrEq(entity.className, "worldspawn")) {
            entity.model = 0;
        }
        uint32_t first = edicts.size();
        edicts.insert(edicts.end(), epairs.cbegin(), epairs.cend());
        for (uint32_t pi = first; pi < edicts.size() - 1; ++pi) {
            edicts[pi].next = &edicts[pi + 1];
        }
        entity.first = &edicts[first];
        entities.push_back(entity);
    };

    while (!Eof()) {
        if (!GetChar('{')) {
            break;
        }
        std::vector<Edict>   epairs;
        do {
            Edict   edict;
            char    string1[MAX_STRING_LENGTH + 1];
            char    string2[MAX_STRING_LENGTH + 1];
            if (!GetString(string1)) {
                break;
            }
            if (!GetString(string2)) {
                return false;
            }
            edict.key = string1;
            edict.value = string2;
            StorePairStrings(edict);
            epairs.push_back(edict);
        } while (!Eof());
        if (Eof() || !GetChar('}')) {
            return false;
        }
        EdictsToEntity(epairs);
    }
    return true;
}

void    Entities::Destroy()
{
    strings.release();
}

const char* Entities::EntityValueStr(const Entity& entity, const char* key)
{
    const Edict*   edict = entity.first;
    while (edict != nullptr) {
        if (StrEq(edict->key, key)) {
            return edict->value;
        }
        edict = edict->next;
    }
    return nullptr;
}

int     Entities::EntityValueFloat(const Entity& entity, const char* key, float* value)
{
    const char*   val = EntityValueStr(entity, key);
    if (val == nullptr) {
        return 0;
    }
    *value = std::atof(val);
    return 1;
}

int     Entities::EntityValueVec3(const Entity& entity, const char* key, Vec3 value)
{
    const char*   val = EntityValueStr(entity, key);
    if (val == nullptr) {
        return 0;
    }
    std::sscanf(val, "%f %f %f", &value[0], &value[1], &value[2]);
    return 1;
}
