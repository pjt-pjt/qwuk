#include "bsp.h"
#include "filebuffer.h"
#include "graphics.h"
#include "glm/gtc/type_ptr.hpp"
#include "game.h"
#include "gamemodule.h"
#include "entities.h"
#include "tools.h"
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


bool    BSP::Init()
{
    bool ok = vertexBuffer.Init();
    ok = ok && lightsBuffer.Init();
    ok = ok && pipeline.Init(vertexBuffer);
    ok = ok && testPipeline.Init(vertexBuffer);
    actEntities.reserve(2048);  //TODO

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
    }
    if (ok) {
        ok = CreateEntities();
#if 0
        printf("Entities:\n%s", bspFile.entities);
#endif
    }
    CreateLights();
    if (ok) {
        vertexBuffer.Use();
        vertexBuffer.SetData<Vertex>(vertices);
        lightsBuffer.Use(1);
        vertexBuffer.Unuse();
        facesToDraw.resize(faces.size(), 0);
    }
    loaded = ok;

    return ok && CheckOK();
}

void    BSP::Close()
{
    if (loaded) {
        graphics.DeleteTextures(textureIds.size(), textureIds.data());

        entities.Destroy();
        actEntities.clear();
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

void    BSP::BeginDraw(const glm::vec3& camera, const glm::mat4& view, const glm::mat4& proj)
{
    static glm::mat4 model(1);
    auto SetUniforms = [&view, &proj] (Pipeline& pipeline) {
        Program& program = pipeline.GetProgram();
        program.SetUniform("view", view);
        program.SetUniform("proj", proj);
        program.SetUniform("model", model);
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
        Program& program = pipeline.GetProgram();
        program.SetUniformInt("numLights", lights.size());
        program.SetUniform("viewPos", camera);
    }
    ++actFrame;
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
    // Draw models for entities, except for triggers
    for (const auto& entity : actEntities) {
        if (entity.model != -1) {
            if (!config.showAll) {
                if (!config.showTriggers && StrPrefix(entity.className, "trigger")) {
                    continue;
                }
                if (!config.showFuncDoors && StrPrefix(entity.className, "func_door")) {
                    continue;
                }
                if (!config.showFuncPlats && StrPrefix(entity.className, "func_plat")) {
                    continue;
                }
                if (!config.showFuncWalls && StrPrefix(entity.className, "func_wall")) {
                    continue;
                }
                if (!config.showFuncEpisodeGate && StrPrefix(entity.className, "func_episodegate")) {
                    continue;
                }
                if (!config.showFuncBossGate && StrPrefix(entity.className, "func_bossgate")) {
                    continue;
                }
            }
            const Model& model = models[entity.model];
            if (test.testModeOn) {
                Program& program = testPipeline.GetProgram();
                program.SetUniform("model", model.transform);
            } else {
                Program& program = pipeline.GetProgram();
                program.SetUniform("model", model.transform);
            }
            CheckOK();
            Draw(model, &nodes[model.firstNode], camera);
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

Content    BSP::TracePoint(const glm::vec3& point)
{
    Content content;
    for (auto& entity : actEntities) {
        if (entity.model != -1) {
            if (!config.showAll) {
                if (!config.showFuncDoors && StrPrefix(entity.className, "func_door")) {
                    continue;
                }
                if (!config.showFuncPlats && StrPrefix(entity.className, "func_plat")) {
                    continue;
                }
                if (!config.showFuncWalls && StrPrefix(entity.className, "func_wall")) {
                    continue;
                }
                if (!config.showFuncEpisodeGate && StrPrefix(entity.className, "func_episodegate")) {
                    continue;
                }
                if (!config.showFuncBossGate && StrPrefix(entity.className, "func_bossgate")) {
                    continue;
                }
            }
            content = TracePoint(models[entity.model], models[entity.model].clipNode, point);
            if (content.content != EMPTY) {
                content.entity = &entity;
            }
            // if (StrPrefix(entity.className, "trigger") && content.content == SOLID) {
            //     content.content = TRIGGER;
            //     content.entity = &entity;
            //     break;
            // }
        }
    }

    return content;
}

bool    BSP::TraceLine(const glm::vec3& start, const glm::vec3& end, Trace& trace)
{
    Trace total;
    total.end = end;
    bool empty;
    // Draw models for entities, except for triggers
    for (auto& entity : actEntities) {
        if (entity.model != -1) {
            if (StrPrefix(entity.className, "trigger")) {
                continue;
            }
            if (!config.showAll) {
                if (!config.showFuncDoors && StrPrefix(entity.className, "func_door")) {
                    continue;
                }
                if (!config.showFuncPlats && StrPrefix(entity.className, "func_plat")) {
                    continue;
                }
                if (!config.showFuncWalls && StrPrefix(entity.className, "func_wall")) {
                    continue;
                }
                if (!config.showFuncEpisodeGate && StrPrefix(entity.className, "func_episodegate")) {
                    continue;
                }
                if (!config.showFuncBossGate && StrPrefix(entity.className, "func_bossgate")) {
                    continue;
                }
            }
            Trace tr;
            tr.end = end;
            tr.allSolid = true;
            empty = TraceLine(models[entity.model], models[entity.model].clipNode, start, end, 0, 1, tr);

            if (tr.allSolid)
                tr.startSolid = true;
            if (tr.startSolid)
                tr.fraction = 0;

            // if (!empty) {
            //     tr.entity = &entity;
            // }
            if (tr.fraction < total.fraction) {
                total = tr;
                total.entity = &entity;
            }
        }
    }
    trace = total;
    Content content = TracePoint(trace.end);
    trace.endContent = content.content;
    if (content.content != EMPTY) {
        trace.entity = content.entity;
    }

    return empty;
}


bool    BSP::CreateEntities()
{
    bool ok = entities.Init(bspFile.entities, bspFile.entitiesSize);
    if (ok) {
        for (auto& entity : entities.entities) {
            if (entity.model != -1) {
                const auto& model = models[entity.model];
                Vec3Copy(entity.mins, glm::value_ptr(model.mins));
                Vec3Copy(entity.maxs, glm::value_ptr(model.maxs));
            }
        }
    }
    return ok;
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

        // // Get plane normal
        const BSPPlane& plane = planes[bface.plane_id];
        glm::vec3       normal = (bface.side == 0) ? plane.Normal() : -plane.Normal();
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
        node.mins = {bnode.box.min[0], bnode.box.min[1], bnode.box.min[2]};
        node.maxs = {bnode.box.max[0], bnode.box.max[1], bnode.box.max[2]};
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
        model.mins = {bmodel.bound.min.x, bmodel.bound.min.y, bmodel.bound.min.z};
        model.maxs = {bmodel.bound.max.x, bmodel.bound.max.y, bmodel.bound.max.z};
        model.transform = glm::mat4(1);
        models.push_back(model);
    }
}

void    BSP::CreateLights()
{
    lights.reserve(100);
    for (const auto& entity : entities.entities) {
        Light   light;
        light.origin = {entity.origin[0], entity.origin[1], entity.origin[2]};
        light.intensity = 300;
        light.range = 300;
        if (StrEq(entity.className, "light")) {
            light.color = {1.0f, 1.0f, 0.8f};
        } else if (StrEq(entity.className, "light_flame_large_yellow")) {
            light.color = {1.0f, 1.0f, 0.6f};
        } else if (StrEq(entity.className, "light_flame_small_yellow")) {
            light.color = {1.0f, 1.0f, 0.6f};
        } else if (StrEq(entity.className, "light_flame_small_white")) {
            light.color = {1.0f, 1.0f, 1.0f};
        } else if (StrEq(entity.className, "light_fluoro") || StrEq(entity.className, "light_fluorospark")) {
            light.color = {1.0f, 1.0f, 1.0f};
        } else if (StrEq(entity.className, "light_torch_small_walltorch")) {
            light.color = {1.0f, 1.0f, 0.6f};
            light.range = 200;
            light.intensity = 200;
        } else {
            continue;
        }
        float   value;
        if (Entities::EntityValueFloat(entity, "light", &value)) {
            light.range = value;
            light.intensity = light.range;
        }
        lights.push_back(light);
    }
    lightsBuffer.SetData<Light>(lights);
    CheckOK();
#if 0
    // Dump lights
    for (uint32_t li = 0; li < lights.size(); ++li) {
        auto& light = lights[li];
        printf("#%3d: intensity %4.2f, range %4.0f\n", li, light.intensity, light.range);
    }
#endif
}

void    BSP::Draw(const Model& model, Node* node, const glm::vec3& camera)
{
    auto DrawFront = [this, &camera, &model](Node* node) {
        if (node->frontNode != nullptr)
            Draw(model, node->frontNode, camera);
        else if (node->frontLeaf != nullptr)
            Draw(node->frontLeaf);
    };
    auto DrawBack = [this, &camera, &model](Node* node) {
        if (node->backNode != nullptr)
            Draw(model, node->backNode, camera);
        else if (node->backLeaf != nullptr)
            Draw(node->backLeaf);
    };
    BSPPlane    plane = *node->plane;
    plane.Transform(model.transform);
    if (plane.Classify(camera) != BSPPlane::Back) {
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
    for (int32_t fli = leaf->firstFace; fli < leaf->firstFace + leaf->numFaces; ++fli) {
        int32_t fi = faceList[fli];
        if (facesToDraw[fi] == actFrame) {
            continue;
        }
        facesToDraw[fi] = actFrame;

        Face& face = faces[fi];
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

Content    BSP::TracePoint(const Model& model, short node, const glm::vec3& point)
{
    while (node >= 0) {
        const ClipNode& cnode = clipNodes[node];
        BSPPlane        plane = *cnode.plane;
        plane.Transform(model.transform);
        if (plane.Classify(point) != BSPPlane::Back) {
            node = cnode.front;
        } else {
            node = cnode.back;
        }
    }
    Content content;
    content.content = LeafType(node);
    return content;
}

bool    BSP::TraceLine(const Model& model, short node, const glm::vec3& start, const glm::vec3& end, float fstart, float fend, Trace& trace)
{
    static constexpr float DIST_EPSILON = 0.03125;

    if (node < 0) {
		if (node != SOLID) {
			trace.allSolid = false;
			// if (node == CONTENTS_EMPTY)
			// 	trace->inopen = true;
			// else
			// 	trace->inwater = true;
		} else {
			trace.startSolid = true;
        }
		return true;		// empty
	}
        
    const ClipNode& cnode = clipNodes[node];
    BSPPlane        plane = *cnode.plane;
    plane.Transform(model.transform);
    float t1 = plane.SignedDistance(start);
    float t2 = plane.SignedDistance(end);
    if (t1 >= 0 && t2 >= 0) {
        return TraceLine(model, cnode.front, start, end, fstart, fend, trace);
    } else if (t1 < 0 && t2 < 0) {
        return TraceLine(model, cnode.back, start, end, fstart, fend, trace);
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

    float       fmid = fstart + frac * (fend - fstart);
    glm::vec3   mid;
    mid = start + fmid * (end - start);

    short front = (t1 >= 0) ? cnode.front : cnode.back;
    short back = (t1 >= 0) ? cnode.back : cnode.front;
    if (!TraceLine(model, front, start, mid, fstart, fmid, trace)) {
        return false;
    }

    if (TracePoint(model, back, mid).content == EMPTY) {
        return TraceLine(model, back, mid, end, fmid, fend, trace);
    }

    if (t1 >= 0) {
        trace.plane = plane;
    } else {
        trace.plane = BSPPlane(-plane.Normal(), -plane.Distance(), plane.GetOrientation());
    }

	while (TracePoint(model, model.clipNode, mid).content == SOLID) {
        // Shouldn't really happen, but does occasionally
		frac -= 0.1;
		if (frac < 0) {
			trace.fraction = fmid;
            trace.end = mid;
			return false;
		}
		fmid = fstart + (fend - fstart) * frac;
        mid = start + frac * (end - start);
	}

    trace.fraction = fmid;
    trace.end = mid;

    return false;
}