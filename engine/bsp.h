#pragma once

#include "vertex.h"
#include "storagebuffer.h"
#include "pipeline.h"
#include "bspfile.h"
#include "filebuffer.h"
#include "entities.h"
#include "plane.h"
#include "game.h"
#include <vector>
#include <string>
#include <optional>


class BSPFile;
class Entity;


struct Config
{
    int     fov = 75;
    bool    alwaysRun = true;
    bool    showAll = false;
    bool    showTriggers = false;
    bool    showFuncDoors = true;
    bool    showFuncPlats = true;
    bool    showFuncWalls = false;
    bool    showFuncTrains = false;
    bool    showFuncIllusionary = true;
    bool    showFuncEpisodeGate = false;
    bool    showFuncBossGate = true;
    bool    smoothTextures = false;
    bool    showEntityInfo = false;
    bool    noclip = false;
};

struct Stats
{
    uint32_t minFPS = INT_MAX;
    uint32_t maxFPS = 0;
    uint32_t FPS = 0;
    uint32_t minFrameTime = INT_MAX;
    uint32_t maxFrameTime = 0;
    uint64_t totalFPS = 0;
    uint32_t frames = 0;
    uint32_t binds = 0;
    uint32_t primitives = 0;
};

struct Test
{
    bool        testModeOn = false;
    enum TestMode { FullBright, Surfaces };
    TestMode    testMode = FullBright;
};


class BSPPlane : public Plane
{
public:
    enum Orientation {
        AxialX,     // 0: Axial plane, in X
        AxialY,     // 1: Axial plane, in Y
        AxialZ,     // 2: Axial plane, in Z
        TowardX,    // 3: Non axial plane, roughly toward X
        TowardY,    // 4: Non axial plane, roughly toward Y
        TowardZ     // 5: Non axial plane, roughly toward Z
    };
    BSPPlane() : Plane(), orientation(AxialZ) {}
    BSPPlane(const glm::vec3& normal, float distance, Orientation orientation) :
        Plane(normal, distance),
        orientation(orientation)
    {}
    Orientation GetOrientation() const
    {
        return orientation;
    }
    void Transform(const glm::mat4& mat)
    {
        Plane::Transform(mat);
        //TODO calculate orientation
    }
private:
    Orientation     orientation;
};

enum LeafType
{
    EMPTY   = -1,
    SOLID   = -2,
    WATER   = -3,
    SLIME   = -4,
    LAVA    = -5,
    SKY     = -6,
};

struct Trace
{
    BSPPlane    plane;
    EntPtr      entity = nullptr;
    float       fraction = 1.0;   
    glm::vec3   end;
    bool        startSolid = false;
    bool        allSolid = false;
};

struct Face
{
    int     vertexIdx;
    int     numVertices;
    int     planeIdx;
    int     texture;
};


struct Light
{
    glm::vec3   origin;
    float       intensity;
    glm::vec3   color;
    float       range;
    int         style;
    float       padding[3];
};


class QVertexBuffer: public VertexBuffer
{
public:
    virtual void    AddAtributes() final;
};

class MainPipeline : public Pipeline
{
public:
    virtual void    AddShaders() final;
};

class TestPipeline : public Pipeline
{
public:
    virtual void    AddShaders() final;
};


class BSP
{
public:
    BSP(Config& config, Stats& stats, Test& test) : config(config), stats(stats), test(test)
    {}
    bool        Init();
    void        Destroy();
    bool        Load(const char* path);
    void        Close();
    void        SetTextureMode(bool smooth);

    void        BeginDraw(const glm::vec3& camera, const glm::mat4& view, const glm::mat4& proj);
    void        Draw(const glm::vec3& camera);
    void        EndDraw();

    LeafType    PlayerTouchEnts(const glm::vec3& point);
    LeafType    PointContent(const glm::vec3& point);
    Trace       PlayerMove(const glm::vec3& start, const glm::vec3& end);
    Trace       TraceLine(const glm::vec3& start, const glm::vec3& end);
    void        TouchEnt(EntPtr entity);

    struct Leaf;
    struct Node
    {
        BSPPlane*   plane = nullptr;
        u_short     front;
        u_short     back;
        glm::vec3   mins;
        glm::vec3   maxs;
    };
    struct Leaf
    {
        int         firstFace = 0;
        int         numFaces = 0;
        LeafType    type;
    };

    struct ClipNode
    {
        BSPPlane*   plane = nullptr;
        short       front;    
        short       back;
    };

    struct Model
    {
        short       firstNode[2];
        glm::vec3   mins;
        glm::vec3   maxs;
        glm::mat4   transform;
    };

private:
    bool        CreateEntities();
    void        CreateTextures();
    void        CreatePlanes();
    void        CreateFaces();
    void        CreateBSP();
    void        CreateClipNodes();
    void        CreateHull0();
    void        CreateModels();
    void        CreateLights();

    void        Draw(const Model& model, u_short node, const glm::vec3& camera);
    void        Draw(const Leaf& leaf);

    template<int hullIndex>
    Trace       TraceLine(const glm::vec3& start, const glm::vec3& end, bool triggers = false);
    struct HullInfo
    {
        const std::vector<ClipNode>&    hull;
        glm::mat4&                      transform;
        short                           firstNode;
    };
    template<int hullIndex>
    LeafType    TracePoint(const glm::vec3& point, bool checkTouchEnts);
    LeafType    TracePoint(const HullInfo& hull, short node, const glm::vec3& point);
    bool        TraceLine(const HullInfo& hull, short node, const glm::vec3& start, const glm::vec3& end, float fstart, float fend, Trace& trace);

private:
    Entities                entities;
    std::vector<Entity>     actEntities;

    int		                numTouch;
    static constexpr int MAX_TOUCHENTS = 32;
	EntPtr                  touchEnts[MAX_TOUCHENTS];
    EntPtr                  lookAtEnt = nullptr;

    std::vector<Vertex>     vertices;
    std::vector<Face>       faces;
    std::vector<BSPPlane>   planes;
    std::vector<Node>       nodes;
    std::vector<Leaf>       leaves;
    std::vector<int>        faceList;
    std::vector<ClipNode>   hulls[2];
    std::vector<Model>      models;
    std::vector<Light>      lights;

    std::vector<uint32_t>   textureIds;

    Config&                 config;
    Stats&                  stats;
    Test&                   test;
    BSPFile                 bspFile;
    FileBuffer              palette;

    QVertexBuffer           vertexBuffer;
    StorageBuffer           lightsBuffer;
    MainPipeline            pipeline;
    TestPipeline            testPipeline;

    std::vector<uint32_t>   facesToDraw;
    uint32_t                actFrame = 0;

    bool                    loaded = false;

    friend class Quake;
    friend class GameInterface;
    friend class PlayerMove;    //TODO
};
