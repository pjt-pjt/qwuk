#pragma once

#include "vertex.h"
#include "pipeline.h"
#include "bspfile.h"
#include "filebuffer.h"
#include "plane.h"
#include <vector>
#include <string>


class BSPFile;


struct Config
{
    int     fov = 75;
    bool    showAll = false;
    bool    showTriggers = false;
    bool    showFuncDoors = true;
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
    enum TestMode { FullBright, Lightmaps, Surfaces };
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


struct Trace {
    BSPPlane    plane;
    glm::vec3   start;
    glm::vec3   end;
    float       fraction = 1.0;   
};


struct Face
{
    int     vertexIdx;
    int     numVertices;
    int     planeIdx;
    int     texture;
    int     lightmap = 0;
};


struct Entity
{
    struct Pair
    {
        std::string key;
        std::string value;
    };
    std::string className;
    glm::vec3   origin = {0, 0, 0};
    float       angle = 0;
    uint32_t    model = 0;
    std::vector<Pair>   pairs;
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
    BSP(Config& config, Stats& stats) : config(config), stats(stats)
    {}
    bool Init();
    void Destroy();
    bool Load(const char* path);
    void Close();
    void SetTextureMode(bool smooth);

    void BeginDraw(const glm::mat4& view, const glm::mat4& proj);
    void Draw(const glm::vec3& camera);
    void EndDraw();

    bool TracePoint(const glm::vec3& point);
    bool TraceLine(const glm::vec3& start, const glm::vec3& end, Trace& trace);

    struct Leaf;
    struct Node
    {
        BSPPlane*   plane = nullptr;
        Node*       frontNode = nullptr;
        Node*       backNode = nullptr;
        Leaf*       frontLeaf = nullptr;
        Leaf*       backLeaf = nullptr;
    };

    enum LeafType {
        EMPTY   = -1,
        SOLID   = -2,
        WATER   = -3,
        SLIME   = -4,
        LAVA    = -5,
        SKY     = -6
    };
    struct Leaf
    {
        int         fistFace = 0;
        int         numFaces = 0;
        LeafType    type;
    };
    struct ClipNode
    {
        BSPPlane*   plane = nullptr;
        short       front;    
        short       back;
    };
    struct Model{
        uint32_t    firstNode;
        uint32_t    clipNode;
    };

    std::vector<Entity>     entities;
    std::vector<Vertex>     vertices;
    std::vector<Face>       faces;
    std::vector<BSPPlane>   planes;
    std::vector<Node>       nodes;
    std::vector<Leaf>       leaves;
    std::vector<int>        faceList;
    std::vector<ClipNode>   clipNodes;
    std::vector<Model>      models;

    std::vector<uint32_t>   textureIds;
    std::vector<uint32_t>   lightmaps;

    Config&                 config;
    Stats&                  stats;
    Test                    test;
    BSPFile                 bspFile;
    FileBuffer              palette;

    QVertexBuffer           vertexBuffer;
    MainPipeline            pipeline;
    TestPipeline            testPipeline;

    bool                    loaded = false;

private:
    bool CreateEntities();
    void CreateTextures();
    void CreatePlanes();
    void CreateFaces();
    void CreateBSP();
    void CreateClipNodes();
    void CreateModels();

    void Draw(Node* node, const glm::vec3& camera);
    void Draw(Leaf* node);

    bool TracePoint(short node, const glm::vec3& point);
    bool TraceLine(short node, const glm::vec3& start, const glm::vec3& end, Trace& trace);
};
