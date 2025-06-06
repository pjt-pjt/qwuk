#pragma once

#include "bsp.h"
#include "gameinterface.h"
#include "SDL2/SDL.h"
#include <string>
#include <queue>
#include <ltdl.h>


struct ImFont;
class  Entity;


class Camera
{
public:
    Camera() :
        position(0, 0, 0),
        eyePosition(0, 0, eyeHeight),
        eyeHeight(0)
    {}
    void        SetPosition(const glm::vec3& pos);
    void        SetYaw(float yaw);
    void        SetPitch(float pitch);
    void        SetEyeHeight(float height);

    const glm::vec3&   Position() const
    {
        return position;
    }
    const float&   Yaw() const
    {
        return yaw;
    }
    const float&   Pitch() const
    {
        return pitch;
    }
    const glm::vec3& EyePosition() const
    {
        return eyePosition;
    }

    glm::vec3        Direction() const;

private:
    glm::vec3   position;
    glm::vec3   eyePosition;
    float       yaw = 0.f;
    float       pitch = 0.f;
    float       eyeHeight = 0;
};

class Actor : public Camera
{
public:
    void Init(const Entity& entity);
    
public:
    //glm::vec3   mins = {-16, -16, -24};
    //glm::vec3   maxs = { 16,  16,  32};
    bool        flying = false;
    bool        onGround = false;
    glm::vec3   direction = { 0, 0, 0 };
    float       speed = 0;
    float       verticalSpeed = 0;
};


struct Command
{
    enum Cmd { None, ChangeMap, Quit };
    Cmd             cmd = None;
    uint32_t        waitFrames = 0;
    std::string     strParam1 = {};
};


class Quake
{
public:
    Quake();
    bool Init(const std::string& map);
    void Destroy();
    void SetViewPort(int w, int h);

    void ProcessKeyboardEvent(const SDL_Event& event);
    void ProcessMouseEvent(const SDL_Event& event);
    void NextFrame(uint64_t elapsed);

    void Render();
    void Pause(bool pause)
    {
        paused = pause;
    }
    bool Paused() const
    {
        return paused;
    }

private:
    bool InitGame(const std::string& map);
    void GUI();

    void MovePlayer(uint64_t elapsed);
    void PlayerFly(const glm::vec3& start, const glm::vec3& end, Trace& trace);
    void PlayerGroundMove(const glm::vec3& start, const glm::vec3& end, Trace& trace);

    void AddCommand(const Command& cmd);
    void DoCommands(uint64_t elapsed);

private:
    Config          config;
    Test            test;
    BSP             bsp;
    bool            loaded = false;
    bool            paused = true;
    int             width = 0;
    int             height = 0;
    glm::mat4       view;
    glm::mat4       proj;
    ImFont*         quakeFontSmall;
    ImFont*         quakeFontLarge;

    Actor           player;
    bool            keyMatrix[256];
    SDL_Scancode    lastKey = SDL_SCANCODE_UNKNOWN;
    float           yawDelta = 0;
    float           pitchDelta = 0;
    glm::vec3       velocity;

    lt_dlhandle     handle = nullptr;
    GameInterface   game;
    std::queue<Command> commands;

public:
    Stats           stats;

    friend class GameInterface;
};
