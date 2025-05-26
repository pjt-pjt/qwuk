#pragma once

#include <cstdint>
#include <string>
#include "SDL2/SDL.h"
#include "quake.h"


#define APP_NAME "qwuk"


class Application
{
public:
    bool Init();
    void Destroy();
    bool Run(const std::string& map);
    void Quit();

    static const char* VersionStr();

private:
    bool InitGraphics();
    bool InitGui();
    void BeginFrame();
    void EndFrame();
    void DestroyGui();
    void DestroyGraphics();

    bool HandleEvents(void);
    void HandleKeyEvent(const SDL_Event& event);
    void HandleMouseEvent(const SDL_Event& event);
    void SetFullScreen(bool full);
    void NextFrame(uint32_t elapsed);
    void Render(uint32_t elapsed);

private:
    SDL_Window*             window = nullptr;
    SDL_GLContext           context = nullptr;
    bool                    fullScreen = false;
    int                     hRes = 1280;
    int                     vRes = 720;
    Quake                   quake;
};


extern bool         debugMode;
extern Application* app;