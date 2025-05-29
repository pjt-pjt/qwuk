#include "bsp.h"
#include "filebuffer.h"
#include "graphics.h"
#include "glm/gtc/type_ptr.hpp"
#include <memory>
#include <sstream>


void    QVertexBuffer::AddAtributes()
{
    uint32_t    stride = sizeof(Vertex);
    AddAtribute(0, 0, 3, stride);
    AddAtribute(1, 3, 2, stride);
    AddAtribute(2, 5, 3, stride);
    AddAtribute(3, 8, 3, stride);
}

void    MainPipeline::AddShaders()
{
    program.Init("shaders/quake.vert", "shaders/quake.frag");
}

void    TestPipeline::AddShaders()
{
    program.Init("shaders/quake-test.vert", "shaders/quake-test.frag");
}


bool    Entity::HasKey(const std::string& key) const
{
    for (const auto& pair : pairs) {
        if (pair.key == key) {
            return true;
        }
    }
    return false;
}

std::optional<const std::string*> Entity::GetValue(const std::string& key) const
{
    for (const auto& pair : pairs) {
        if (pair.key == key) {
            return &pair.value;
        }
    }
    return {};
}


bool    BSP::Init()
{
    bool ok = vertexBuffer.Init();
    ok = ok && lightsBuffer.Init();
    ok = ok && pipeline.Init(vertexBuffer);
    ok = ok && testPipeline.Init(vertexBuffer);

    return ok;
}

void    BSP::Destroy()
{
    Close();
    pipeline.Destroy();
    testPipeline.Destroy();
    lightsBuffer.Destroy();
    vertexBuffer.Destroy();
    CheckOK();
}

bool    BSP::Load(const char* path)
{
    Close();
    bool ok = bspFile.Init(path);
    if (ok) {
        ok = CreateEntities();
#if 0
        printf("Entities:\n%s", bspFile.entities);
#endif
    }
    if (ok) {
        // LoadPalette
        if (!palette.Init("resources/palette.lmp")) {
            return false;
        }
        if (palette.Size() != 768) {
            return false;
        }
        CreateTextures();
    }
    if (ok) {
        CreatePlanes();
        CreateFaces();
        CreateBSP();
        CreateClipNodes();
        CreateModels();
        CreateLights();
    }
    if (ok) {
        vertexBuffer.Use();
        vertexBuffer.SetData<Vertex>(vertices);
        vertexBuffer.Unuse();
    }
    loaded = ok;

    return ok;
}

void    BSP::Close()
{
    if (loaded) {
        graphics.DeleteTextures(textureIds.size(), textureIds.data());

        entities.clear();
        vertices.clear();
        faces.clear();
        planes.clear();
        nodes.clear();
        leaves.clear();
        faceList.clear();
        clipNodes.clear();
        models.clear();
        lights.clear();
    }
    loaded = false;
}

void    BSP::SetTextureMode(bool smooth)
{
    for (auto txId : textureIds) {
        Graphics::Texture tex;
        if (smooth) {
            graphics.ChangeTexture(txId, tex, Graphics::Linear, Graphics::Linear, Graphics::MipLinear);
        } else {
            graphics.ChangeTexture(txId, tex, Graphics::Nearest, Graphics::Nearest, Graphics::MipNearest);
        }
    }
}

void    BSP::BeginDraw(const glm::mat4& view, const glm::mat4& proj)
{
    auto SetUniforms = [&view, &proj] (Pipeline& pipeline) {
        Program& program = pipeline.GetProgram();
        program.SetUniform("view", view);
        program.SetUniform("proj", proj);
        program.SetUniformInt("tex", 0);
        CheckOK();
    };
    graphics.BeginDraw();
    if (test.testModeOn) {
        testPipeline.Use();
        SetUniforms(testPipeline);
        testPipeline.GetProgram().SetUniformInt("testMode", test.testMode);
        CheckOK();
    } else {
        pipeline.Use();
        SetUniforms(pipeline);
    }
}


static uint32_t color = 0x404040;

void    BSP::Draw(const glm::vec3& camera)
{
    stats.binds = 0;
    stats.primitives = 0;
    if (!loaded) {
        return;
    }

    // Draw world
    color = 0x404040;
    const Model& model = models[0];
    Draw(&nodes[model.firstNode], camera);
    // Draw models for entities, except for triggers
    for (const auto& entity : entities) {
        if (entity.model != 0) {
            if (!config.showAll) {
                if (!config.showTriggers && entity.className.find("trigger") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncDoors && entity.className.find("func_door") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncWalls && entity.className.find("func_wall") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncEpisodeGate && entity.className.find("func_episodegate") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncBossGate && entity.className.find("func_bossgate") != std::string::npos) {
                    continue;
                }
            }
            const Model& model = models[entity.model];
            Draw(&nodes[model.firstNode], camera);
        }
    }
}

void    BSP::EndDraw()
{
    if (test.testModeOn) {
        testPipeline.Unuse();
    } else {
        pipeline.Unuse();
    }
    stats.binds = graphics.GetBindsCount();
}

bool    BSP::TracePoint(const glm::vec3& point)
{
    const Model& model = models[0];
    bool empty = TracePoint(model.clipNode, point);
    // Draw models for entities, except for triggers
    for (const auto& entity : entities) {
        if (!empty) {
            break;
        }
        if (entity.model != 0) {
            if (entity.className.find("trigger") != std::string::npos) {
                continue;
            }
            if (!config.showAll) {
                if (!config.showFuncDoors && entity.className.find("func_door") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncWalls && entity.className.find("func_wall") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncEpisodeGate && entity.className.find("func_episodegate") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncBossGate && entity.className.find("func_bossgate") != std::string::npos) {
                    continue;
                }
            }
            empty = TracePoint(models[entity.model].clipNode, point);
        }
    }

    return empty;
}

bool    BSP::TraceLine(const glm::vec3& start, const glm::vec3& end, Trace& trace)
{
    const Model& model = models[0];
    bool empty = TraceLine(model.clipNode, start, end, trace);
    // Draw models for entities, except for triggers
    for (const auto& entity : entities) {
        if (!empty) {
            break;
        }
        if (entity.model != 0) {
            if (entity.className.find("trigger") != std::string::npos) {
                continue;
            }
            if (!config.showAll) {
                if (!config.showFuncDoors && entity.className.find("func_door") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncWalls && entity.className.find("func_wall") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncEpisodeGate && entity.className.find("func_episodegate") != std::string::npos) {
                    continue;
                }
                if (!config.showFuncBossGate && entity.className.find("func_bossgate") != std::string::npos) {
                    continue;
                }
            }
            empty = TraceLine(models[entity.model].clipNode, start, end, trace);
        }
    }

    return empty;
}


bool    BSP::CreateEntities()
{
    std::string entityStr = bspFile.entities;
    std::basic_istringstream iss(entityStr);

    auto GetChar = [&iss](char gc) -> bool {
        char c;
        iss.get(c);
        while ((c == ' ' || c == '\n') && !iss.eof()) {
            iss.get(c);
        }
        return !iss.eof() && (c == gc);
    };
    auto GetString = [&iss, &GetChar](std::string& string) -> bool {
        if (!GetChar('\"')) {
            iss.unget();
            return false;
        }
        char c;
        iss.get(c);
        while (c != '\"' && !iss.eof()) {
            string.append(1, c);
            iss.get(c);
        }
        if (!iss.eof() && c != '\"') {
            iss.unget();
        }
        return !iss.eof();
    };
    auto PairsToEntity = [this] (const std::vector<Entity::Pair>& pairs) {
        Entity  entity;
        auto Search = [&pairs] (const std::string& key) -> int {
            for (uint32_t i = 0; i < pairs.size(); ++i) {
                if (pairs[i].key == key) {
                    return i;
                }
            }
            return -1;
        };
        int idx = Search("classname");
        if (idx == -1) {
            return;
        }
        entity.className = pairs[idx].value;
        idx = Search("origin");
        if (idx != -1) {
            float x, y, z;
            std::sscanf(pairs[idx].value.c_str(), "%f %f %f", &x, &y, &z);
            entity.origin = {x, y, z};
        }
        idx = Search("angle");
        if (idx != -1) {
            std::sscanf(pairs[idx].value.c_str(), "%f", &entity.angle);
        }
        idx = Search("model");
        if (idx != -1) {
            std::sscanf(pairs[idx].value.c_str(), "*%u", &entity.model);
        }
        entity.pairs = pairs;
        entities.push_back(entity);
    };

    while (!iss.eof()) {
        if (!GetChar('{')) {
            break;
        }
        std::vector<Entity::Pair>   pairs;
        do {
            Entity::Pair    pair;
            if (!GetString(pair.key)) {
                break;
            }
            if (!GetString(pair.value)) {
                return false;
            }
            pairs.push_back(pair);
        } while (!iss.eof());
        if (iss.eof() || !GetChar('}')) {
            return false;
        }
        PairsToEntity(pairs);
    }
    return true;
}

void    BSP::CreateTextures()
{
    uint32_t    MissingTexture[64 * 64];
    for (uint32_t i = 0; i < 64; ++i) {
        for (uint32_t j = 0; j < 64; ++j) {
            MissingTexture[i * 64 + j] = (((i / 16) % 2) ^ ((j / 16) % 2)) ? 0xffff00ff : 0xff000000;
        }
    }   

    textureIds.resize(bspFile.textures->numtex + 1);
    graphics.GenTextures(bspFile.textures->numtex + 1, textureIds.data());

    Graphics::Texture   tx;
    tx.width = 64;
    tx.height = 64;
    tx.components = 4;
    tx.data = MissingTexture;
    graphics.CreateTexture(textureIds[0], tx, Graphics::Nearest, Graphics::Nearest, Graphics::MipNearest);

    for (uint32_t ti = 0; ti < bspFile.textures->numtex; ++ti) {
        if (bspFile.textures->offset[ti] == -1) {
            continue;
        }
        uint32_t        buffer[256 * 256];
        miptex_t*       miptex = (miptex_t*)(((u_char*)bspFile.textures) + bspFile.textures->offset[ti]);
        u_char*         txPtr = ((u_char*)miptex) + miptex->offset1;
        const u_char*   colors = palette.Get<u_char>();
        for (uint32_t tp = 0; tp < miptex->width * miptex->height; ++tp, ++txPtr) {
            uint32_t color = 0xff;
            color <<= 8;
            color |= colors[(*txPtr * 3) + 2];
            color <<= 8;
            color |= colors[(*txPtr * 3) + 1];
            color <<= 8;
            color |= colors[(*txPtr * 3)];
            buffer[tp] = color;
        }

        Graphics::Texture   tx;
        tx.width = miptex->width;
        tx.height = miptex->height;
        tx.components = 4;
        tx.data = buffer;
        graphics.CreateTexture(textureIds[ti + 1], tx, Graphics::Nearest, Graphics::Nearest, Graphics::MipNearest);
    }
    assert(CheckOK());
}

void    BSP::CreatePlanes()
{
    // Set planes
    planes.reserve(bspFile.numPlanes);
    for (uint32_t pi = 0; pi < bspFile.numPlanes; ++pi) {
        plane_t& plane = bspFile.planes[pi];
        planes.push_back(BSPPlane(glm::vec3(plane.normal.x, plane.normal.y, plane.normal.z), plane.dist, BSPPlane::Orientation(plane.type)));
    }
}

void    BSP::CreateFaces()
{
    faces.reserve(bspFile.numFaces);

    for (uint32_t fi = 0; fi < bspFile.numFaces; ++fi) {
        face_t& bface = bspFile.faces[fi];
        Face face;
        face.vertexIdx = vertices.size();
        face.numVertices = bface.ledge_num;
        face.planeIdx = bface.plane_id;
        surface_t& texInfo = bspFile.texInfos[bface.texinfo_id];
        face.texture = texInfo.texture_id + 1;
        miptex_t& texture = *(miptex_t*)(((char*)bspFile.textures) + bspFile.textures->offset[texInfo.texture_id]);

        vertices.resize(vertices.size() + face.numVertices);
        u_char light = 0xff - bface.baselight;
        Color color(light, light, light);

        int offs = face.numVertices;
        for (int32_t lei = bface.ledge_id; lei < bface.ledge_id + bface.ledge_num; ++lei) {
            int ei = bspFile.edgeList[lei];
            bool flip = ei < 0;
            if (flip) {
                ei = -ei;
            }
            edge_t& bedge = bspFile.edges[ei];
            --offs;
            Vertex& vertex = vertices[face.vertexIdx + offs];
            if (flip) {
                vertex.pos = glm::vec3(bspFile.vertices[bedge.vertex1].X, bspFile.vertices[bedge.vertex1].Y, bspFile.vertices[bedge.vertex1].Z);
            } else {
                vertex.pos = glm::vec3(bspFile.vertices[bedge.vertex0].X, bspFile.vertices[bedge.vertex0].Y, bspFile.vertices[bedge.vertex0].Z);
            }
            float s = glm::dot(vertex.pos, glm::vec3(texInfo.vectorS.x, texInfo.vectorS.y, texInfo.vectorS.z)) + texInfo.distS;    
            float t = glm::dot(vertex.pos, glm::vec3(texInfo.vectorT.x, texInfo.vectorT.y, texInfo.vectorT.z)) + texInfo.distT;
            vertex.uv = {s / texture.width, t / texture.height};            
            vertex.color = color;
        }
        // Calculate normal
        glm::vec3  normal(0);
        for (int32_t vi = 0; vi < face.numVertices - 2 && glm::length(normal) < SMALL_EPS; ++vi) {
            glm::vec3& v0 = vertices[face.vertexIdx + vi].pos;
            glm::vec3& v1 = vertices[face.vertexIdx + vi + 1].pos;
            glm::vec3& v2 = vertices[face.vertexIdx + (vi + 2) % face.numVertices].pos;
            normal = cross(v1 - v0, v2 - v1);
        }
        for (int32_t vi = 0; vi < face.numVertices; ++vi) {
            vertices[face.vertexIdx + vi].normal = normal;
        }
        faces.push_back(face);
    }
}

void    BSP::CreateBSP()
{
    nodes.resize(bspFile.numNodes);
    leaves.resize(bspFile.numLeaves);
    faceList.resize(bspFile.numFaceList);
    for (uint32_t ni = 0; ni < bspFile.numNodes; ++ni) {
        node_t& bnode = bspFile.nodes[ni];
        Node& node = nodes[ni];
        if ((bnode.front & 0x8000) == 0) {
            node.frontNode = &nodes[bnode.front];
        } else if (bnode.front != 65535) {
            u_short idx = ~bnode.front;
            node.frontLeaf = &leaves[idx];
        }
        if ((bnode.back & 0x8000) == 0) {
            node.backNode = &nodes[bnode.back];
        } else if (bnode.back != 65535) {
            u_short idx = ~bnode.back;
            node.backLeaf = &leaves[idx];
        }
        node.plane = &planes[bnode.plane_id];
    }
    for (uint32_t li = 0; li < bspFile.numLeaves; ++li) {
        dleaf_t& dleaf = bspFile.leaves[li];
        Leaf& leaf = leaves[li];
        leaf.firstFace = dleaf.lface_id;
        leaf.numFaces  = dleaf.lface_num;
        leaf.type      = LeafType(dleaf.type);
    }
    for (uint32_t fi = 0; fi < bspFile.numFaceList; ++fi) {
        faceList[fi] = bspFile.faceList[fi];
    }
}

void    BSP::CreateClipNodes()
{
    clipNodes.reserve(bspFile.numClipnodes);
    for (uint32_t ci = 0; ci < bspFile.numClipnodes; ++ci) {
        const clipnode_t&  bclip = bspFile.clipnodes[ci];
        ClipNode clip;
        clip.plane = &planes[bclip.planenum];
        clip.front = bclip.front;
        clip.back  = bclip.back;
        clipNodes.push_back(clip);
    }
}

void    BSP::CreateModels()
{
    models.reserve(bspFile.numModels);
    for (uint32_t mi = 0; mi < bspFile.numModels; ++mi) {
        const model_t&  bmodel = bspFile.models[mi];
        Model model;
        model.firstNode = bmodel.node_id0;
        model.clipNode = bmodel.node_id1;
        models.push_back(model);
    }
}

void    BSP::CreateLights()
{
    lights.reserve(100);
    for (const auto& entity : entities) {
        Light   light;
        light.origin = entity.origin;
        light.intensity = 2.0;
        light.range = 200;
        if (entity.className == "light") {
            light.style = 0;
        } else {
            continue;
        }
        Entity::Result result = entity.GetValue("light");
        if (result) {
            light.range = std::stof(**result);
            light.intensity = light.range / 100.0f;
        }
        lights.push_back(light);
    }
    lightsBuffer.SetData<Light>(lights);
}

void    BSP::Draw(Node* node, const glm::vec3& camera)
{
    auto DrawFront = [this, &camera](Node* node) {
        if (node->frontNode != nullptr)
            Draw(node->frontNode, camera);
        else if (node->frontLeaf != nullptr)
            Draw(node->frontLeaf);
    };
    auto DrawBack = [this, &camera](Node* node) {
        if (node->backNode != nullptr)
            Draw(node->backNode, camera);
        else if (node->backLeaf != nullptr)
            Draw(node->backLeaf);
    };
    if (node->plane->Classify(camera) != BSPPlane::Back) {
        DrawFront(node);
        DrawBack(node);
    } else {
        DrawBack(node);
        DrawFront(node);
    }
}

void    BSP::Draw(Leaf* leaf)
{
    if (leaf == nullptr || leaf->type == SOLID) {
        return;
    }
    for (int32_t fi = leaf->firstFace; fi < leaf->firstFace + leaf->numFaces; ++fi) {
        Face& face = faces[faceList[fi]];
        //if (test.textModeOn)
        if (test.testModeOn && test.testMode == Test::Surfaces) {
            Color fcolor(color);
            testPipeline.GetProgram().SetUniform("faceColor", fcolor.r, fcolor.g, fcolor.b);

            Color fcolor2;
            fcolor2.r = fcolor.g;
            fcolor2.g = fcolor.b;
            fcolor2.b = fcolor.r;
            color = fcolor2;
            color += 4;
        }

        graphics.BindTexture(0, textureIds[face.texture]);
        graphics.DrawTrangles(Graphics::TrinangleFan, face.vertexIdx, face.numVertices);
        ++stats.primitives;
    }
}

bool    BSP::TracePoint(short node, const glm::vec3& point)
{
    while (node >= 0) {
        const ClipNode& cnode = clipNodes[node];
        if (cnode.plane->Classify(point) != BSPPlane::Back) {
            node = cnode.front;
        } else {
            node = cnode.back;
        }
    }
    if (LeafType(node) == EMPTY) {
        return true;
    } else {
        return false;
    }
}

bool    BSP::TraceLine(short node, const glm::vec3& start, const glm::vec3& end, Trace& trace)
{
    static constexpr float DIST_EPSILON = 0.03125;

    if (node >= 0) {
        const ClipNode& cnode = clipNodes[node];
        float t1 = cnode.plane->SignedDistance(start);
        float t2 = cnode.plane->SignedDistance(end);
        if (t1 >= 0 && t2 >= 0) {
            return TraceLine(cnode.front, start, end, trace);
        } else if (t1 <= 0 && t2 <= 0) {
            return TraceLine(cnode.back, start, end, trace);
        }

        float frac;
        if (t1 < 0) {
            frac = (t1 + DIST_EPSILON)/(t1-t2);
        } else {
            frac = (t1 - DIST_EPSILON)/(t1-t2);
        }
        if (frac < 0) {
            frac = 0;
        }
        if (frac > 1) {
            frac = 1;
        }

        float       midf = frac;
        glm::vec3   mid;
        mid = start + frac * (end - start);

        short front = (t1 >= 0) ? cnode.front : cnode.back;
        short back = (t1 >= 0) ? cnode.back : cnode.front;
        if (!TraceLine(front, start, mid, trace)) {
            return false;
        }

        if (TracePoint(back, mid)) {
            return TraceLine(back, mid, end, trace);
        }

        if (t1 >= 0) {
            trace.plane = *cnode.plane;
        } else {
            trace.plane = BSPPlane(-cnode.plane->Normal(), -cnode.plane->Distance(), cnode.plane->GetOrientation());
        }

        trace.fraction = midf;
    }
    return node == EMPTY;
}