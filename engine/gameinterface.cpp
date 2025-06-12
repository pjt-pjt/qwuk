#include "gameinterface.h"
#include "quake.h"
#include "game.h"


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
}

void    GameInterface::AddCommand(int command, const char* strParam1, float /* fltParam1 */, int /* intParam1 */)
{
    if (game == nullptr) {
        return;
    }
    game->quake.AddCommand({Command::Cmd(command), 2, strParam1});
}
