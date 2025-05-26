#pragma once


class Quake;
struct Functions;

class GameInterface
{
public:
    GameInterface(Quake& quake);
    ~GameInterface();

    bool    Init(Functions* funcitons);

private:
    static void    AddCommand(int command, const char* strParam1, float fltParam1, int intParam1);

private:
    Quake&      quake;
};
