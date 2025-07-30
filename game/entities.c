#include "entities.h"
#include "tools.h"
#include "game.h"
#include <stdio.h>
#include <math.h>


Fields  fieldList[2048];
int     numFields;


void    InfoPlayerStart(EntPtr ent);
void    TriggerOnce(EntPtr ent);
void    TriggerMultiple(EntPtr ent);
void    TriggerCounter(EntPtr ent);
void    TriggerTeleport(EntPtr ent);
void    TriggerChangelevel(EntPtr ent);
void    TriggerOnlyRegistered(EntPtr ent);
void    FuncDoor(EntPtr ent);
void    FuncDoorSecret(EntPtr ent);
void    FuncButton(EntPtr ent);
void    FuncPlat(EntPtr ent);


void    ResetFields()
{
    numFields = 0;
}

Fields* NewFields()
{
    Fields* fields = &fieldList[numFields++];
    memset(fields, 0, sizeof(Fields));
    return fields;
}


void    Construct(EntPtr entity)
{
    const char* Classes[] = {
        "worldspawn",
        "info_player_start",
        "info_player_start2",
        "trigger_once",
        "trigger_multiple",
        "trigger_counter",
        "trigger_teleport",
        "trigger_changelevel",
        "trigger_onlyregistered",
        "func_door",
        "func_door_secret",
        "func_button",
        "func_plat",
        NULL
    };
    void (*Constructors[])(EntPtr self) = {
        Spawn,
        InfoPlayerStart,
        InfoPlayerStart,
        TriggerOnce,
        TriggerMultiple,
        TriggerCounter,
        TriggerTeleport,
        TriggerChangelevel,
        TriggerOnlyRegistered,
        FuncDoor,
        FuncDoorSecret,
        FuncButton,
        FuncPlat
    };

    for (int cl = 0; Classes[cl] != NULL; ++cl) {
        if (StrEq(entity->className, Classes[cl]) && Constructors[cl] != NULL) {
            Constructors[cl](entity);
            return;
        }
    }

    //Hack construct all necessary
    if (StrPrefix(entity->className, "info_") ||
        StrPrefix(entity->className, "trigger_") ||
        StrPrefix(entity->className, "func_"))
    {
        i.Spawn(entity);
    }
}

int     Touching(EntPtr ent1, EntPtr ent2)
{
    if (ent1->maxs[0] < ent2->mins[0]) {
        return 0;
    }
    if (ent1->mins[0] > ent2->maxs[0]) {
        return 0;
    }

    if (ent1->maxs[1] < ent2->mins[1]) {
        return 0;
    }
    if (ent1->mins[1] > ent2->maxs[1]) {
        return 0;
    }

    if (ent1->maxs[2] < ent2->mins[2]) {
        return 0;
    }
    if (ent1->mins[2] > ent2->maxs[2]) {
        return 0;
    }
    return 1;
}

void    Spawn(EntPtr entity)
{
    i.Spawn(entity);
}

void    InfoPlayerStart(EntPtr ent)
{
    if (StrEq(globals->map, "start")) {
        if (globals->status == 0 && StrEq(ent->className, "info_player_start2")) {
            return;
        } else if (globals->status > 0 && !StrEq(ent->className, "info_player_start2")) {
            return;
        }
    }
    EntPtr self = i.Spawn(ent);
    Vec3Set(self->mins, -16, -16, -24);
    Vec3Set(self->maxs,  16,  16,  32);
    self->eyePos = 22;
    ++self->origin[2];
    i.SpawnPlayer(self);
}


void    UseTargets(EntPtr self)
{
    const char* target = i.EntityValueStr(self, "target");
    if (target != NULL) {
        EntPtr  ent = i.SearchEntity(NULL, NULL, "targetname", target);
        while (ent != NULL) {
            if (ent != self && ent->Use != NULL) {
                ent->Use(ent, self);
            }
            ent = i.SearchEntity(ent, NULL, "targetname", target);
        }
    }
}
