#include "quake.h"
#include "application.h"
#include "graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "game.h"
#include <filesystem>
#include <ltdl.h>


void    Camera::SetPosition(const glm::vec3& pos)
{
    position = pos;
    eyePosition = position + glm::vec3(0, 0, eyeHeight);
}

void    Camera::SetYaw(float yaw_)
{
    auto NormalizeAngle = [](float& angle) {
        if (angle < -SMALL_EPS) {
            while (angle < -SMALL_EPS) {
                angle += 360.0;
            }
        } else if (angle > SMALL_EPS) {
            while (angle - 360.0 > SMALL_EPS) {
                angle -= 360.0;
            }
        } else {
            angle = 0;
        }
    };

    yaw = yaw_;
    NormalizeAngle(yaw);
}

void    Camera::SetPitch(float pitch_)
{
    auto FixPitch = [](float& angle) {
        if (angle < -89) {
            angle = -89;
        } else if (angle > 89) {
            angle = 89;
        }
    };
    pitch = pitch_;
    FixPitch(pitch);
}

glm::vec3   Camera::Direction() const
{
    float p = pitch * glm::pi<float>() / 180.0;
    float y = yaw * glm::pi<float>() / 180.0;
    float xzLen = cos(p);
    return glm::vec3(cos(y) * xzLen, sin(y) * xzLen, sin(p));
}


Quake::Quake() :
    bsp(config, stats),
    velocity(0),
    game(*this)
{
    for (auto& key : keyMatrix) {
        key = false;
    }
}

bool    Quake::Init(const std::string& map)
{
    lt_dlinit();

    bool ok = bsp.Init();
    ImGui::GetIO().Fonts->AddFontDefault();
    quakeFontSmall = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/dpquake_.ttf", 18.0);
    quakeFontLarge = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/dpquake_.ttf", 36.0);

    ok = ok && InitGame(map);

    return ok;
}

void    Quake::Destroy()
{
    bsp.Destroy();
    lt_dlclose(handle);
    lt_dlexit();
}

void    Quake::SetViewPort(int w, int h)
{
    width = w;
    height = h;
    graphics.SetViewport(width, height);
    proj = glm::perspective(config.fov * PI / 180.0f, float(width) / float(height), .1f, 16384.f);
}

void    Quake::ProcessKeyboardEvent(const SDL_Event& event)
{
    if (event.type == event.key.repeat || event.key.keysym.scancode > 255) {
        return;
    } else if (event.type == SDL_KEYDOWN) {
        keyMatrix[event.key.keysym.scancode] = true;
        switch (event.key.keysym.scancode)
        {
            // case SDL_SCANCODE_LALT:
            // case SDL_SCANCODE_RALT:
            // case SDL_SCANCODE_LSHIFT:
            // case SDL_SCANCODE_RSHIFT:
            // case SDL_SCANCODE_LCTRL:
            // case SDL_SCANCODE_RCTRL:
            //     modifiers.Set(event.key.keysym.scancode);
            //     break;
            default:
                if (lastKey == SDL_SCANCODE_UNKNOWN) {
                    lastKey = event.key.keysym.scancode;
                }
                break;
        }
    } else if (event.type == SDL_KEYUP) {
        keyMatrix[event.key.keysym.scancode] = false;
        switch (event.key.keysym.scancode)
        {
            // case SDL_SCANCODE_LALT:
            // case SDL_SCANCODE_RALT:
            // case SDL_SCANCODE_LSHIFT:
            // case SDL_SCANCODE_RSHIFT:
            // case SDL_SCANCODE_LCTRL:
            // case SDL_SCANCODE_RCTRL:
            //     modifiers.Reset(event.key.keysym.scancode);
            //     break;
            default:
                if (lastKey == event.key.keysym.scancode) {
                    lastKey = SDL_SCANCODE_UNKNOWN;
                }
                break;
        }
    }    
}

void    Quake::ProcessMouseEvent(const SDL_Event& event)
{
    if (event.type == SDL_MOUSEMOTION && !paused) {
        yawDelta -= (float)event.motion.xrel / 2.5f;
        pitchDelta -= (float)event.motion.yrel / 2.5f;
    }    
}

void    Quake::NextFrame(uint64_t elapsed)
{
    if (lastKey != SDL_SCANCODE_UNKNOWN) {
        if (lastKey == SDL_SCANCODE_F11) {
            config.noclip = !config.noclip;
        }
        lastKey = SDL_SCANCODE_UNKNOWN;
    }
    DoCommands(elapsed);
    MovePlayer(elapsed);
}

void    Quake::Render()
{
    graphics.Clear();
    GUI();
    if (loaded) {
        glm::vec3 pos = player.EyePosition();
        glm::vec3 center = pos + player.Direction();
        view = glm::lookAt(pos, center, glm::vec3(0,0,1));
        bsp.BeginDraw(view, proj);
        bsp.Draw(pos);
        bsp.EndDraw();
    }
}

bool    Quake::InitGame(const std::string& map)
{
    handle = lt_dlopen("libgame.dll");
    bool ok = (handle != NULL);
    if (ok) {
        InitProc    GameInit;
        GameInit = (InitProc)lt_dlsym(handle, "Init");
        static Functions   functions;
        game.Init(&functions);
        if (GameInit == NULL || GameInit(&functions) != 0) {
            ok = false;
        }

        if (ok) {
            StartProc   GameStart;
            GameStart = (StartProc)lt_dlsym(handle, "Start");
            if (GameStart == NULL) {
                ok = false;
            }
            GameStart(!map.empty() ? map.c_str() : nullptr);
        }
    }
    return ok;
}


static bool inSolid = false;

void    Quake::GUI()
{
    ImGui::Begin("Settings");
        if (ImGui::SliderInt("FOV", &config.fov, 75, 100)) {
            SetViewPort(width, height);
        }
        ImGui::Separator();
        ImGui::Checkbox("Show All", &config.showAll);
        ImGui::BeginDisabled(config.showAll);
        ImGui::Checkbox("Show triggers", &config.showTriggers);
        ImGui::Checkbox("Show Func Doors", &config.showFuncDoors);
        ImGui::Checkbox("Show Func Walls", &config.showFuncWalls);
        ImGui::Checkbox("Show Episode Gates", &config.showFuncEpisodeGate);
        ImGui::Checkbox("Show Boss Gate", &config.showFuncBossGate);
        ImGui::EndDisabled();
        ImGui::Separator();
        if (ImGui::Checkbox("Smooth Textures", &config.smoothTextures)) {
            bsp.SetTextureMode(config.smoothTextures);
        }
        ImGui::Checkbox("Noclip", &config.noclip);

        ImGui::Separator();
        static std::vector<std::string>     fileList;
        static int32_t                      selectedMapFile = -1;
        if (ImGui::Button("Open map...")) {
            fileList.clear();
            namespace fs = std::filesystem;
            std::string path = "maps/";
            for (const auto& entry : fs::directory_iterator(path)) {
                fileList.push_back(entry.path().filename().string());
            }
            ImGui::OpenPopup("select_map_file");
        }
        if (ImGui::BeginPopup("select_map_file")) {
            ImGui::SeparatorText("maps/");
            for (uint32_t i = 0; i < fileList.size(); ++i)
                if (ImGui::Selectable(fileList[i].c_str())) {
                    selectedMapFile = i;
                }
            ImGui::EndPopup();
        }
        if (selectedMapFile > -1) {
            AddCommand({Command::ChangeMap, 2, "maps/" + fileList[selectedMapFile]});
            selectedMapFile = -1;
        }

        ImGui::Separator();
        {
            ImGui::Checkbox("Test on", &bsp.test.testModeOn);
        }
        {
            ImGui::BeginDisabled(!bsp.test.testModeOn);
            const char* testValues[] = {"Full Bright", "Surfaces"};
            ImGui::Text("Debug: %s", testValues[bsp.test.testMode]);
            int testMode = bsp.test.testMode;
            ImGui::SliderInt("##Debug Value", &testMode, 0, 1);
            bsp.test.testMode = Test::TestMode(testMode);
            ImGui::EndDisabled();
        }
        ImGui::Text("v%s", Application::VersionStr());
    ImGui::End();

    ImGui::Begin("Stats");
        ImGui::Text("Min Frame time %5u", stats.minFrameTime);
        ImGui::Text("Max Frame time %5u", stats.maxFrameTime);
        ImGui::Text("Min FPS        %5u", stats.minFPS);
        ImGui::Text("Max FPS        %5u", stats.maxFPS);
        ImGui::Separator();
        ImGui::Text("FPS            %5u", stats.FPS);
        ImGui::Text("Binds          %5u", stats.binds);
        ImGui::Text("Primitives     %5u", stats.primitives);
        ImGui::Separator();
        ImGui::Text("Node: %s", inSolid ? "Solid" : "Empty");
    ImGui::End();

    if (paused) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::PushFont(quakeFontSmall);
        ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::PushFont(quakeFontLarge);
            if (ImGui::Button("    Start   ")) {
                AddCommand({Command::ChangeMap, 2, "maps/start.bsp"});
                loaded = false;
            }
            if (ImGui::Button("    Quit    ")) {
                app->Quit();
            }
            ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }
}

void    Quake::MovePlayer(uint64_t elapsed)
{
    if (paused) {
        return;
    }
    float secondsElapsed = float(elapsed) / 1000.0f;
    float speed = 256 * secondsElapsed;
    if (keyMatrix[SDL_SCANCODE_W]) {
        velocity.x =  speed;
    } else if (keyMatrix[SDL_SCANCODE_S]) {
        velocity.x = -speed;
    } else {
        velocity.x =  0;
    }

    if (keyMatrix[SDL_SCANCODE_A]) {
        velocity.y =  speed;
    } else if (keyMatrix[SDL_SCANCODE_D]) {
        velocity.y = -speed;
    } else {
        velocity.y =  0;
    }

    if (keyMatrix[SDL_SCANCODE_Q]) {
        velocity.z =  speed;
    } else if (keyMatrix[SDL_SCANCODE_X]) {
        velocity.z = -speed;
    } else {
        velocity.z =  0;
    }
    player.SetYaw(player.Yaw() + yawDelta);
    player.SetPitch(player.Pitch() + pitchDelta);
    pitchDelta = yawDelta = 0;

    if (glm::length(velocity) > SMALL_EPS) {
        Trace trace;
        glm::mat4 mat = glm::rotate(glm::mat4(1), player.Yaw() * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
        trace.start = player.Position();
        trace.end = player.Position() + glm::vec3(mat * glm::vec4(velocity, 0));
        trace.fraction = 1;

        //if fly or noclip
        PlayerFly(trace);
    }
}

void    Quake::PlayerFly(Trace& trace)
{
    if (config.noclip || bsp.TraceLine(trace.start, trace.end, trace)) {
        player.SetPosition(trace.end);
    }
}

void    Quake::AddCommand(const Command& cmd)
{
    commands.push(cmd);
}

void    Quake::DoCommands(uint64_t /* elapsed */)
{
    if (!commands.empty()) {
        static bool working = false;
        if (working) {
            return;
        }
        working = true;
        Command& cmd = commands.front();
        if (cmd.waitFrames > 0) {
            --cmd.waitFrames;
        } else {
            if (cmd.cmd == Command::ChangeMap) {
                loaded = bsp.Load(cmd.strParam1.c_str());
                if (loaded) {
                    for (const auto& entity : bsp.entities) {
                        if (entity.className == "info_player_start") {
                            player.SetPosition(entity.origin);
                            player.SetYaw(entity.angle);
                            player.SetPitch(0);
                        }
                    }
                    //Close menu
                    paused = false;   //TODO Run map
                }    
            }
            commands.pop();
        }
        working = false;
    }
}
