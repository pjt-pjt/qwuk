#pragma once

#include "entity.h"
#include "game.h"


void    ResetFields();
void    Construct(EntPtr entity);
int     Touching(EntPtr ent1, EntPtr ent2);
void    Spawn(EntPtr entity);
Fields* NewFields();

void    UseTargets(EntPtr self);

extern Variables* globals;
