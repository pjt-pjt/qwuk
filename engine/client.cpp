#include "client.h"
#include "application.h"
#include "graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "game.h"
#include "tools.h"
#include "bsp.h"
#include <filesystem>


void    Camera::SetPosition(const glm::vec3& pos)
{
    position = pos;
    SetEyeHeight(eyeHeight);
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

void    Camera::SetEyeHeight(float height)
{
    eyeHeight = height;
    eyePosition = position + glm::vec3(0, 0, eyeHeight);
}

glm::vec3   Camera::Direction() const
{
    float p = pitch * glm::pi<float>() / 180.0;
    float y = yaw * glm::pi<float>() / 180.0;
    float xzLen = cos(p);
    return glm::vec3(cos(y) * xzLen, sin(y) * xzLen, sin(p));
}


void    Actor::Init(Entity& ent)
{
    if (StrPrefix(ent.className,"info_player_start")) {
        SetPosition({ent.origin[0], ent.origin[1], ent.origin[2]});
        SetYaw(ent.angle);
        SetEyeHeight(ent.eyePos);
        mins = {ent.mins[0], ent.mins[1], ent.mins[2]};
        maxs = {ent.maxs[0], ent.maxs[1], ent.maxs[2]};
        entity = &ent;
    }
}


Quake::Quake() :
    bsp(config, stats, test),
    playerMove(bsp, player),
    velocity(0),
    interface(*this)
{
    for (auto& key : keyMatrix) {
        key = false;
    }
}

bool    Quake::Init(const std::string& map)
{
    bool ok = bsp.Init();
    ImGui::GetIO().Fonts->AddFontDefault();
    quakeFontSmall = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/dpquake_.ttf", 18.0);
    quakeFontLarge = ImGui::GetIO().Fonts->AddFontFromFileTTF("resources/dpquake_.ttf", 36.0);

    ok = ok && InitGame(map);

    test.testModeOn = testMode;

    return ok;
}

void    Quake::Destroy()
{
    bsp.Destroy();
    game.Destroy();
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
    if (event.type == SDL_MOUSEMOTION && status == Running) {
        yawDelta -= (float)event.motion.xrel / 2.5f;
        pitchDelta -= (float)event.motion.yrel / 2.5f;
    }    
}

void    Quake::NextFrame(uint64_t elapsed)
{
    if (lastKey != SDL_SCANCODE_UNKNOWN) {
        if (lastKey == SDL_SCANCODE_F11) {
            config.noclip = !config.noclip;
        } else if (lastKey == SDL_SCANCODE_F) {
            player.flying = !player.flying;
        }
        lastKey = SDL_SCANCODE_UNKNOWN;
    }
    float secondsElapsed = elapsed / 1000.f;
    DoCommands(secondsElapsed);
    DoEntities(secondsElapsed);
    MovePlayer(secondsElapsed);
}

void    Quake::Render()
{
    graphics.Clear();
    GUI();
    if (status > Loading) {
        glm::vec3 pos = player.EyePosition();
        glm::vec3 center = pos + player.Direction();
        view = glm::lookAt(pos, center, glm::vec3(0,0,1));
        bsp.BeginDraw(pos, view, proj);
        bsp.Draw(pos);
        bsp.EndDraw();
    }
}

bool    Quake::InitGame(const std::string& map)
{
    bool ok = game.InitModule("libquake.dll", interface);
    if (ok) {
        //TODO Set globals
        globals.status = 0;
        game.Run(!map.empty() ? map.c_str() : nullptr);
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
        ImGui::Checkbox("Always Run", &config.alwaysRun);
        ImGui::Separator();
        ImGui::Checkbox("Show All", &config.showAll);
        ImGui::BeginDisabled(config.showAll);
        ImGui::Checkbox("Show triggers", &config.showTriggers);
        ImGui::Checkbox("Show Func Doors", &config.showFuncDoors);
        ImGui::Checkbox("Show Func Plats", &config.showFuncPlats);
        ImGui::Checkbox("Show Func Walls", &config.showFuncWalls);
        ImGui::Checkbox("Show Episode Gates", &config.showFuncEpisodeGate);
        ImGui::Checkbox("Show Boss Gate", &config.showFuncBossGate);
        ImGui::EndDisabled();
        ImGui::Separator();
        if (ImGui::Checkbox("Smooth Textures", &config.smoothTextures)) {
            bsp.SetTextureMode(config.smoothTextures);
        }
        ImGui::Checkbox("Fly mode", &player.flying);
        ImGui::SameLine();
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
            PostCommand({Command::ChangeMap, 2, "maps/" + fileList[selectedMapFile]});
            selectedMapFile = -1;
        }

        ImGui::Separator();
        {
            ImGui::Checkbox("Test on", &test.testModeOn);
        }
        {
            ImGui::BeginDisabled(!test.testModeOn);
            const char* testValues[] = {"Full Bright", "Surfaces"};
            ImGui::Text("Debug: %s", testValues[test.testMode]);
            int testMode = test.testMode;
            ImGui::SliderInt("##Debug Value", &testMode, 0, 1);
            test.testMode = Test::TestMode(testMode);
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
        ImGui::Text("Player: %6.2f %6.2f %6.2f", player.Position().x, player.Position().y, player.Position().z);
        ImGui::Text("Node: %s", inSolid ? "Solid" : "Empty");
        ImGui::Text("%s", player.onGround ? "On ground" : "In air");
    ImGui::End();

    if (status == Menu) {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::PushFont(quakeFontSmall);
        ImGui::Begin("Menu", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
            ImGui::PushFont(quakeFontLarge);
            if (ImGui::Button("    Start   ")) {
                PostCommand({Command::ChangeMap, 2, "maps/start.bsp"});
            }
            if (ImGui::Button("    Quit    ")) {
                app->Quit();
            }
            ImGui::PopFont();
        ImGui::End();
        ImGui::PopFont();
    }
}

void    Quake::PostCommand(const Command& cmd)
{
    commands.push(cmd);
}

void    Quake::DoCommands(float /* elapsed */)
{
    if (!commands.empty()) {
        static bool working = false;
        if (working) {
            return;
        }
        working = true;
        Command& cmd = commands.front();
        if (cmd.cmd == Command::ChangeMap) {
            status = Loading;
        }
        if (cmd.waitFrames > 0) {
            --cmd.waitFrames;
        } else {
            if (cmd.cmd == Command::ChangeMap) {
                const char* map = cmd.strParam1.c_str();
                bool loaded = bsp.Load(map);
                if (!loaded) {
                    map = "maps/start.bsp";
                    loaded = bsp.Load(map);
                }
                if (loaded) {
                    //TODO
                    playerMove.SetVelocity({0, 0, 0});
                    static char mapName[256];
                    strncpy(mapName, map, 256);
                    char* name = strrchr(mapName, '/');
                    char* ext  = strrchr(mapName, '.');
                    if (ext != nullptr ) {
                        *ext = '\0';
                    }
                    globals.map = (name != nullptr) ? ++name : mapName;
                    game.ChangeMap();
                    status = Running;
                }    
            }
            commands.pop();
        }
        working = false;
    }
}

void    Quake::DoEntities(float elapsed)
{
    for (auto& ent : bsp.actEntities) {
        if (ent.wait != -1) {
            ent.wait -= elapsed;
            if (ent.wait <= 0) {
                ent.wait = -1;
                game.Think(&ent);
            }
        }
    }
}

void    Quake::MovePlayer(float elapsed)
{
    if (status != Running) {
        return;
    }
    float walkSpeed = 200;
    float runSpeed = 400;
    bool running = keyMatrix[SDL_SCANCODE_LSHIFT] || config.alwaysRun;
    float speed = (running ? runSpeed : walkSpeed);
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

    bool flyMode = player.flying || config.noclip;
    bool jump = false;
    if (flyMode) {
        if (keyMatrix[SDL_SCANCODE_SPACE]) {
            velocity.z =  speed;
        } else if (keyMatrix[SDL_SCANCODE_C]) {
            velocity.z = -speed;
        } else {
            velocity.z =  0;
        }
    } else {
        if (keyMatrix[SDL_SCANCODE_SPACE]) {
            jump = true;
        }
    }
    player.SetYaw(player.Yaw() + yawDelta);
    player.SetPitch(player.Pitch() + pitchDelta);
    pitchDelta = yawDelta = 0;

    playerMove.SetKeys(jump, keyMatrix[SDL_SCANCODE_E]);
    playerMove.Use();
    if (config.noclip) {
        if (glm::length(velocity) > SMALL_EPS) {
            glm::mat4 mat = glm::rotate(glm::mat4(1), player.Yaw() * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
            glm::vec3 end = player.Position() + glm::vec3(mat * glm::vec4(velocity, 0) * elapsed);
            player.SetPosition(end); 
        }
    } else if (player.flying) {
        playerMove.Fly(velocity, elapsed);
        player.SetPosition(playerMove.Origin());
    } else {
        playerMove.Move(velocity, elapsed);
        player.SetPosition(playerMove.Origin());
        if (playerMove.useEnt != nullptr) {
            game.Use(playerMove.useEnt, player.entity);
        }
        if (playerMove.numTouch > 0) {
            //TODO Not public members
            for (int i = 0; i < playerMove.numTouch; ++i) {
                game.Touch(playerMove.touchEnts[i], player.entity);
            }
        }
    }
}

void    Quake::SetYaw(float yaw)
{
    float rotate = yaw - player.Yaw();
    player.SetYaw(yaw);
    glm::vec3   velocity = playerMove.Velocity();
    glm::mat4 mat = glm::rotate(glm::mat4(1), rotate * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
    velocity = glm::vec3(mat * glm::vec4(velocity, 0));
    playerMove.SetVelocity(velocity);
}
