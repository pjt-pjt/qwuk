#include "quake.h"
#include "application.h"
#include "graphics.h"
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "game.h"
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


void    Actor::Init(const Entity& entity)
{
    if (entity.className == "info_player_start") {
        SetPosition(entity.origin);
        SetYaw(entity.angle);
        SetEyeHeight(22);
        mins = {-16, -16, -24};
        maxs = { 16,  16,  32};
    }
}


Quake::Quake() :
    bsp(config, stats, test),
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
        } else if (lastKey == SDL_SCANCODE_F) {
            player.flying = !player.flying;
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
        bsp.BeginDraw(pos, view, proj);
        bsp.Draw(pos);
        bsp.EndDraw();
    }
}

bool    Quake::InitGame(const std::string& map)
{
    bool ok = game.Init("libgame.dll", interface);
    if (ok) {
        game.Start(!map.empty() ? map.c_str() : nullptr);
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
            AddCommand({Command::ChangeMap, 2, "maps/" + fileList[selectedMapFile]});
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
    float runSpeed = 320 * secondsElapsed;
    float fallSpeed = 2 * runSpeed;
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
    if (flyMode && keyMatrix[SDL_SCANCODE_Q]) {
        velocity.z =  speed;
    } else if (flyMode && keyMatrix[SDL_SCANCODE_X]) {
        velocity.z = -speed;
    } else {
        velocity.z =  0;
    }
    player.SetYaw(player.Yaw() + yawDelta);
    player.SetPitch(player.Pitch() + pitchDelta);
    pitchDelta = yawDelta = 0;

    bool toMove = glm::length(velocity) > SMALL_EPS;
    if (player.flying || config.noclip) {
        if (toMove) {
            glm::mat4 mat = glm::rotate(glm::mat4(1), player.Yaw() * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
            glm::vec3 start = player.Position();
            glm::vec3 end = player.Position() + glm::vec3(mat * glm::vec4(velocity, 0));
            Trace trace;
            PlayerFly(start, end, trace);
        }
    } else {
        // check on ground
        glm::vec3 gravityStart = player.Position();
        glm::vec3 gravityEnd = player.Position() + glm::vec3(0, 0, -1);
        Trace traceGravity;
        if (!bsp.TraceLine(gravityStart, gravityEnd, traceGravity)) {
            player.onGround = true;
        } else {
            player.onGround = false;
            // Can we step down?
            glm::vec3 stepStart = player.Position();
            glm::vec3 stepEnd = player.Position() + glm::vec3(0, 0, -fallSpeed);
            Trace stepTrace;
            if (!bsp.TraceLine(stepStart, stepEnd, stepTrace)) {
                player.onGround = true;
            }
            player.SetPosition(stepTrace.end);
        }
        //
        if (toMove) {
            glm::mat4 mat = glm::rotate(glm::mat4(1), player.Yaw() * glm::pi<float>() / 180.0f, {0, 0, 1.0f});
            glm::vec3 start = player.Position();
            glm::vec3 end = player.Position() + glm::vec3(mat * glm::vec4(velocity, 0));
            Trace trace;
            PlayerGroundMove(start, end, trace);
            if (trace.entity != -1) {
                Collision(player, trace.entity);
            }
        }
        //
    }
}

void    Quake::PlayerFly(const glm::vec3& start, const glm::vec3& end, Trace& trace)
{
    if (config.noclip || bsp.TraceLine(start, end, trace) || trace.fraction > SMALL_EPS) {
        player.SetPosition(config.noclip ? end : trace.end);
    }
}

void    Quake::PlayerGroundMove(const glm::vec3& start, const glm::vec3& end, Trace& trace)
{
    glm::vec3   actStart = start;
    glm::vec3   actEnd = end;

    trace.startContent = bsp.TracePoint(actStart).content;
    bsp.TraceLine(actStart, actEnd, trace);

    uint32_t    bumps = 0;
    while (trace.fraction < 1 && bumps < 2/*MAXBUMPS*/) {
        float oldFraction = trace.fraction;
        if (trace.plane.GetOrientation() != BSPPlane::AxialZ && trace.plane.GetOrientation() != BSPPlane::TowardZ)  {
            // Did wew hit a stair
            Trace   stepTrace;
            glm::vec3 stepStart = actStart;
            stepStart.z += 18 + 1;
            glm::vec3 stepEnd = actEnd;
            stepEnd.z += 19;
            if (bsp.TraceLine(stepStart, stepEnd, stepTrace) || (stepTrace.fraction > oldFraction && stepTrace.endContent != SOLID)) {
                trace = stepTrace;
            }
        } else if (trace.plane.GetOrientation() == BSPPlane::TowardZ) {
            glm::vec3   dir = actEnd - actStart;
            glm::vec3   projected = trace.plane.Normal() * glm::dot(dir, trace.plane.Normal());
            dir = dir - projected;

            Trace   slopeTrace;
            glm::vec3 slopeStart = actStart;
            glm::vec3 slopeEnd = slopeStart + dir;
            if (bsp.TraceLine(slopeStart, slopeEnd, slopeTrace) || (slopeTrace.fraction > oldFraction && slopeTrace.endContent != SOLID)) {
                trace = slopeTrace;
            }
        }
        if (trace.fraction < 1 && trace.plane.GetOrientation() != BSPPlane::AxialZ && trace.plane.GetOrientation() != BSPPlane::TowardZ) {
            float       speed = glm::length(actEnd - actStart) * (1 - trace.fraction);
            glm::vec3   dir = glm::normalize(actEnd - actStart);
            glm::vec3   projected = trace.plane.Normal() * glm::dot(dir, trace.plane.Normal());
            dir = dir - projected;

            actStart = trace.end;
            actEnd = actStart + dir * speed;
            bsp.TraceLine(actStart, actEnd, trace);
        }
        ++bumps;
    }
    if (trace.fraction > SMALL_EPS && trace.endContent != SOLID) {
        player.SetPosition(trace.end);
    }
}

void    Quake::Collision(Actor& /* actor */, int32_t entityIdx)
{
    Entity& entity = bsp.Entities()[entityIdx];
    if (entity.className == "trigger_changelevel") {
        Entity::Result  map = entity.GetValue("map");
        AddCommand({Command::ChangeMap, 2, "maps/" + *map.value() + ".bsp"});
    } else {
        game.Collision(entityIdx);
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
                if (!loaded) {
                    loaded = bsp.Load("maps/start.bsp");
                }
                if (loaded) {
                    for (const auto& entity : bsp.Entities()) {
                        if (entity.className == "info_player_start") {
                            player.Init(entity);
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
