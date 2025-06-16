#pragma once

#include "vertex.h"
#include "storagebuffer.h"
#include "pipeline.h"
#include "bspfile.h"
#include "filebuffer.h"
#include "plane.h"
#include <vector>
#include <string>
#include <optional>


class BSPFile;
class Entity;


struct Config
{
    int     fov = 75;
    bool    showAll = false;
    bool    showTriggers = false;
    bool    showFuncDoors = true;
    bool    showFuncPlats = true;
    bool    showFuncWalls = false;
    bool    showFuncEpisodeGate = false;
    bool    showFuncBossGate = true;
    bool    smoothTextures = false;
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
    TRIGGER = -1000
};

struct Trace
{
    BSPPlane    plane;
    int32_t     entity = -1;
    float       fraction = 1.0;   
    glm::vec3   end;
    LeafType    startContent = SOLID;
    LeafType    endContent = SOLID;
};

struct Content
{
    LeafType    content;
    int32_t     entity = -1;
};


struct Face
{
    int     vertexIdx;
    int     numVertices;
    int     planeIdx;
    int     texture;
};


struct Pairs
{
    struct Pair
    {
        std::string key;
        std::string value;
    };
    std::vector<Pair>   pairs;

    static constexpr uint32_t NotFound = UINT32_MAX;
    uint32_t            FindKey(const std::string& key, uint32_t from, uint32_t count) const;
    const std::string&  GetValue(uint32_t index) const;

    void Append(const std::vector<Pair>& epairs);
};


struct Entity
{
    std::string     className;
    glm::vec3       origin = {0, 0, 0};
    float           angle = 0;
    static constexpr uint32_t NoModel = UINT32_MAX;
    uint32_t        model = NoModel;
    const Pairs&    pairs;
    uint32_t        first = 0;
    uint32_t        count = 0;

    Entity(const Pairs& pairs) : pairs(pairs) {}

    using Value = const std::string&;
    Value   GetValue(const std::string& key) const;
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

    Content     TracePoint(const glm::vec3& point);
    bool        TraceLine(const glm::vec3& start, const glm::vec3& end, Trace& trace);

    const std::vector<Entity>&  Entities() const
    {
        return entities;
    }
    std::vector<Entity>&  Entities()
    {
        return entities;
    }

    struct Leaf;
    struct Node
    {
        BSPPlane*   plane = nullptr;
        Node*       frontNode = nullptr;
        Node*       backNode = nullptr;
        Leaf*       frontLeaf = nullptr;
        Leaf*       backLeaf = nullptr;
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
        uint32_t    firstNode;
        uint32_t    clipNode;
    };

private:
    bool        CreateEntities();
    void        CreateTextures();
    void        CreatePlanes();
    void        CreateFaces();
    void        CreateBSP();
    void        CreateClipNodes();
    void        CreateModels();
    void        CreateLights();

    void        Draw(Node* node, const glm::vec3& camera);
    void        Draw(Leaf* node);

    Content     TracePoint(short node, const glm::vec3& point);
    bool        TraceLine(short node, const glm::vec3& start, const glm::vec3& end, float fstart, float fend, Trace& trace);

private:
    Pairs                   pairs;
    std::vector<Entity>     entities;

    std::vector<Vertex>     vertices;
    std::vector<Face>       faces;
    std::vector<BSPPlane>   planes;
    std::vector<Node>       nodes;
    std::vector<Leaf>       leaves;
    std::vector<int>        faceList;
    std::vector<ClipNode>   clipNodes;
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
};
