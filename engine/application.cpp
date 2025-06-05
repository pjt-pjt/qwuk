#include "application.h"
#include "SDL2/SDL_image.h"
#include "graphics.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "quake.h"
#include "version.h"


bool testMode = false;


bool    Application::Init()
{
    bool ok = InitGraphics();
    ok = ok && InitGui();
    app = ok ? this : nullptr;

    return ok;
}

void Application::Destroy()
{
    DestroyGui();
    DestroyGraphics();
    app = nullptr;
}


static bool         quit = false;

bool    Application::Run(const std::string& map)
{
    if (!quake.Init(map)) {
        return false;
    }
    quake.SetViewPort(hRes, vRes);

    static bool relativeMode = true;
    SDL_SetRelativeMouseMode(SDL_TRUE);

    uint32_t startTime = SDL_GetTicks();
    while (true) {
        uint32_t elapsed = SDL_GetTicks() - startTime;
        if (elapsed == 0) {
            continue;
        }
        startTime = SDL_GetTicks();
        if (!HandleEvents()) {
            break;
        }

        NextFrame(elapsed);
        bool relative = !quake.Paused();
        if (relative != relativeMode) {
            relativeMode = relative;
            SDL_SetRelativeMouseMode(relativeMode ? SDL_TRUE : SDL_FALSE);
        }

        uint32_t renderStart = SDL_GetTicks();
        Render(elapsed);
        ++quake.stats.frames;
        uint32_t renderTime = SDL_GetTicks() - renderStart;
        if (renderTime > quake.stats.maxFrameTime) {
            quake.stats.maxFrameTime = renderTime;
        }
        if (renderTime < quake.stats.minFrameTime) {
            quake.stats.minFrameTime = renderTime;
        }
        quake.stats.FPS = renderTime > 0 ? 1000 / renderTime : 1000;
        quake.stats.totalFPS += quake.stats.FPS;
        if (quake.stats.FPS > quake.stats.maxFPS) {
            quake.stats.maxFPS = quake.stats.FPS;
        }
        if (quake.stats.FPS < quake.stats.minFPS) {
            quake.stats.minFPS = quake.stats.FPS;
        }
    }
    if (debugMode) {
        printf ("Frame time min: %d, max: %d\n", quake.stats.minFrameTime, quake.stats.maxFrameTime);
        printf ("       FPS min: %d, max: %d\n", quake.stats.minFPS, quake.stats.maxFPS);
        printf ("       FPS average:      %d\n", uint32_t(quake.stats.totalFPS / quake.stats.frames));
    }
    quake.Destroy();
    return true;
}

void    Application::Quit()
{
    quit = true;
}

const char* Application::VersionStr()
{
    return APP_VERSION;
}

bool    Application::HandleEvents(void)
{
    SDL_Event event;
    while (SDL_PollEvent (&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type) {
            case SDL_QUIT:
                return false;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                HandleKeyEvent(event);
                break;
            case SDL_MOUSEMOTION:
            case SDL_MOUSEBUTTONDOWN:
            case SDL_MOUSEBUTTONUP:
            case SDL_MOUSEWHEEL:
                HandleMouseEvent(event);
                break;
            default:
                break;
        }
    }
    return !quit;
}

void    Application::HandleKeyEvent(const SDL_Event& e)
{
    if (e.key.repeat != 0) {
        return;
    }
    static bool altEnterDown = false;
    if (!altEnterDown && e.key.keysym.scancode == SDL_SCANCODE_RETURN && (e.key.keysym.mod & KMOD_LALT) != 0) {
        fullScreen = !fullScreen;
        SetFullScreen (fullScreen);
        altEnterDown = true;
        return;
    }
    altEnterDown = false;

    if (e.type == SDL_KEYDOWN && e.key.repeat == 0 && e.key.keysym.sym == SDLK_ESCAPE) {
        quake.Pause(!quake.Paused());
    } else{
        quake.ProcessKeyboardEvent(e);
    }
}

void    Application::HandleMouseEvent(const SDL_Event& e)
{
    quake.ProcessMouseEvent(e);
}

void    Application::SetFullScreen(bool full)
{
    if (full) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
        SDL_DisplayMode mode;
        SDL_GetWindowDisplayMode(window, &mode);
        hRes = mode.w;
        vRes = mode.h;
    } else {
        SDL_SetWindowFullscreen(window, 0);
        SDL_DisplayMode mode;
        SDL_GetWindowDisplayMode(window, &mode);
        hRes = mode.w;
        vRes = mode.h;
    }
    quake.SetViewPort(hRes, vRes);
}

void    Application::NextFrame(uint32_t elapsed)
{
    quake.NextFrame(elapsed);
}

void    Application::Render(uint32_t /* elapsed */)
{
    BeginFrame();
    quake.Render();
#if defined(IMGUI_DEMO)
    ImGui::ShowDemoWindow(); // Show demo window! :)
#endif
    EndFrame();
}

Application* app = nullptr;