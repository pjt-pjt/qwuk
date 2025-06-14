#pragma once


class Quake;
struct Functions;

class GameInterface
{
public:
    GameInterface(Quake& quake);
    ~GameInterface();

    void    Init(Functions* funcitons);

private:
    static void         AddCommand(int command, const char* strParam1, float fltParam1, int intParam1);
    static const char*  EntityClass(int entity);
    static const char*  EntityValueStr(int entity, const char* key);
    static int          EntityValueFloat(int entity, const char* key, float* value);
    static int          EntityValueVec3(int entity, const char* key, float* value);

    static int          SearchEntity(const char* className, const char* key, const char* value);

    static void         SetPlayer(const float* origin, float angle);

private:
    Quake&      quake;
};
