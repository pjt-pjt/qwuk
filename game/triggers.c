#include "tools.h"
#include "game.h"
#include "entities.h"


void    TouchTeleport(EntPtr self, EntPtr other)
{
    const char* target = i.EntityValueStr(self, "target");
    if (target != NULL && i.EntityValueStr(self, "targetname") != NULL)
    {
        return;
    }
    EntPtr targetEnt = i.SearchEntity(NULL, "info_teleport_destination", "targetname", target);
    if (targetEnt != NULL) {
        Vec3 origin;
        Vec3Copy(origin, targetEnt->origin);
        float angle = targetEnt->angle;
        origin[2] -= other->mins[2];
        ++origin[2];
        i.SetPlayerOrigin(origin);
        i.SetPlayerAngle(angle);
    }
}

void    TriggerTeleport(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchTeleport;
}


void    TouchChangelevel(EntPtr self, EntPtr other)
{
    UNUSED(other);
    if (globals->status == 0) {
        globals->status = 1;
    }
    const char* map = i.EntityValueStr(self, "map");
    char path[1024] = "";
    strcat(path, "maps/");
    strcat(path, map);
    strcat(path, ".bsp");
    i.PostCommand(1, path, 0, 0);
}

void    TriggerChangelevel(EntPtr ent)
{
    EntPtr self = i.Spawn(ent);
    self->Touch = TouchChangelevel;
}
