#pragma once

#include "filebuffer.h"
#include <cstdint>


typedef uint32_t u_long;
typedef uint16_t u_short;
typedef uint8_t  u_char;


// Header

struct dentry_t                 // A Directory entry
{
    u_long  offset;            // Offset to entry, in bytes, from start of file
    u_long  size;              // Size of entry in file, in bytes
};


constexpr long Version = 0x1d;  // BSP29

struct dheader_t                // The BSP file header
{
    long  version;              // Model version, must be 0x17 (23).
    dentry_t entities;          // List of Entities.
    dentry_t planes;            // Map Planes.
                                // numplanes = size/sizeof(plane_t)
    dentry_t miptex;            // Wall Textures.
    dentry_t vertices;          // Map Vertices.
                                // numvertices = size/sizeof(vertex_t)
    dentry_t visilist;          // Leaves Visibility lists.
    dentry_t nodes;             // BSP Nodes.
                                // numnodes = size/sizeof(node_t)
    dentry_t texinfo;           // Texture Info for faces.
                                // numtexinfo = size/sizeof(texinfo_t)
    dentry_t faces;             // Faces of each surface.
                                // numfaces = size/sizeof(face_t)
    dentry_t lightmaps;         // Wall Light Maps.
    dentry_t clipnodes;         // clip nodes, for Models.
                                // numclips = size/sizeof(clipnode_t)
    dentry_t leaves;            // BSP Leaves.
                                // numleaves = size/sizeof(leaf_t)
    dentry_t lface;             // List of Faces.
    dentry_t edges;             // Edges of faces.
                                // numedges = Size/sizeof(edge_t)
    dentry_t ledges;            // List of Edges.
    dentry_t models;            // List of Models.
                                // nummodels = Size/sizeof(model_t)
};


// Data

typedef float scalar_t;         // Scalar value,

struct vec3_t                   // Vector or Position
{
    scalar_t x;                 // horizontal
    scalar_t y;                 // horizontal
    scalar_t z;                 // vertical
};

struct boundbox_t               // Bounding Box, Float values
{
    vec3_t   min;               // minimum values of X,Y,Z
    vec3_t   max;               // maximum values of X,Y,Z
};

struct bboxshort_t              // Bounding Box, Short values
{
    short   min[3];             // minimum values of X,Y,Z
    short   max[3];             // maximum values of X,Y,Z
};


// Models

struct model_t
{
    boundbox_t  bound;          // The bounding box of the Model
    vec3_t      origin;         // origin of model, usually (0,0,0)
    long        node_id0;       // index of first BSP node
    long        node_id1;       // index of the first Clip node
    long        node_id2;       // index of the second Clip node
    long        node_id3;       // usually zero
    long        numleafs;       // number of BSP leaves
    long        face_id;        // index of Faces
    long        face_num;       // number of Faces
};


// BSP

struct vertex_t
{
    float X;                    // X,Y,Z coordinates of the vertex
    float Y;                    // usually some integer value
    float Z;                    // but coded in floating point
};

struct edge_t
{
    u_short vertex0;            // index of the start vertex
                                //  must be in [0,numvertices[
    u_short vertex1;            // index of the end vertex
                                //  must be in [0,numvertices[
};

struct face_t
{
    u_short plane_id;           // The plane in which the face lies
                                //           must be in [0,numplanes[ 
    u_short side;               // 0 if in front of the plane, 1 if behind the plane
    long ledge_id;              // first edge in the List of edges
                                //           must be in [0,numledges[
    u_short ledge_num;          // number of edges in the List of edges
    u_short texinfo_id;         // index of the Texture info the face is part of
                                //           must be in [0,numtexinfos[ 
    u_char typelight;           // type of lighting, for the face
    u_char baselight;           // from 0xFF (dark) to 0 (bright)
    u_char light[2];            // two additional light models  
    long lightmap;              // Pointer inside the general light map, or -1
                                // this define the start of the face light map
};

struct node_t
{
    long    plane_id;           // The plane that splits the node
                                //           must be in [0,numplanes[
    u_short front;              // If bit15==0, index of Front child node
                                // If bit15==1, ~front = index of child leaf
    u_short back;               // If bit15==0, id of Back child node
                                // If bit15==1, ~back =  id of child leaf
    bboxshort_t box;            // Bounding box of node and all childs
    u_short face_id;            // Index of first Polygons in the node
    u_short face_num;           // Number of faces in the node
};

struct dleaf_t
{
    long type;                  // Special type of leaf
    long vislist;               // Beginning of visibility lists
                                //     must be -1 or in [0,numvislist[
    bboxshort_t bound;          // Bounding box of the leaf
    u_short lface_id;           // First item of the list of faces
                                //     must be in [0,numlfaces[
    u_short lface_num;          // Number of faces in the leaf  
    u_char sndwater;            // level of the four ambient sounds:
    u_char sndsky;              //   0    is no sound
    u_char sndslime;            //   0xFF is maximum volume
    u_char sndlava;             //
};

struct plane_t
{
    vec3_t normal;              // Vector orthogonal to plane (Nx,Ny,Nz)
                                // with Nx2+Ny2+Nz2 = 1
    scalar_t dist;              // Offset to plane, along the normal vector.
                                // Distance from (0,0,0) to the plane
    long type;                  // Type of plane, depending on normal vector.
                                // 0: Axial plane, in X
                                // 1: Axial plane, in Y
                                // 2: Axial plane, in Z
                                // 3: Non axial plane, roughly toward X
                                // 4: Non axial plane, roughly toward Y
                                // 5: Non axial plane, roughly toward Z
};

struct mipheader_t              // Mip texture list header
{
    u_long   numtex;            // Number of textures in Mip Texture list
    long     offset[1];         // Offset to each of the individual texture
};

struct miptex_t                 // Mip Texture
{
    char   name[16];            // Name of the texture.
    u_long width;               // width of picture, must be a multiple of 8
    u_long height;              // height of picture, must be a multiple of 8
    u_long offset1;             // offset to u_char Pix[width   * height]
    u_long offset2;             // offset to u_char Pix[width/2 * height/2]
    u_long offset4;             // offset to u_char Pix[width/4 * height/4]
    u_long offset8;             // offset to u_char Pix[width/8 * height/8]
};

struct surface_t
{
    vec3_t   vectorS;           // S vector, horizontal in texture space)
    scalar_t distS;             // horizontal offset in texture space
    vec3_t   vectorT;           // T vector, vertical in texture space
    scalar_t distT;             // vertical offset in texture space
    u_long   texture_id;        // Index of Mip Texture
                                //           must be in [0,numtex[
    u_long   animated;          // 0 for ordinary textures, 1 for water 
};

struct clipnode_t
{
    u_long planenum;            // The plane which splits the node
    short front;                // If positive, id of Front child node
                                // If -2, the Front part is inside the model
                                // If -1, the Front part is outside the model
    short back;                 // If positive, id of Back child node
                                // If -2, the Back part is inside the model
                                // If -1, the Back part is outside the model
};


class BSPFile
{
public:
    char*           entities     = nullptr;
    u_long          entitiesSize = 0;
    vertex_t*       vertices     = nullptr;
    u_long          numVertices  = 0;
    node_t*         nodes        = nullptr;
    u_long          numNodes     = 0;
    dleaf_t*        leaves       = nullptr;
    u_long          numLeaves    = 0;
    face_t*         faces        = nullptr;
    u_long          numFaces     = 0;
    u_short*        faceList     = nullptr;
    u_long          numFaceList  = 0;
    edge_t*         edges        = nullptr;
    u_long          numEdges     = 0;
    int*            edgeList     = nullptr;
    u_long          numEdgeList  = 0;
    model_t*        models       = nullptr;
    u_long          numModels    = 0;
    plane_t*        planes       = nullptr;
    u_long          numPlanes    = 0;
    clipnode_t*     clipnodes    = nullptr;
    u_long          numClipnodes = 0;
    mipheader_t*    textures     = nullptr;
    u_long          texturesSize = 0;
    surface_t*      texInfos     = nullptr;
    u_long          numTexinfos  = 0;
    u_char*         lightmaps    = nullptr;
    u_long          lightmapsSize= 0;
/*
    dentry_t visilist;          // Leaves Visibility lists.
*/
public:
    bool    Init(const char* file);

private:
    bool    LoadLumps();
    template<typename T>
    T*      GetBlob(const dentry_t& entry, u_long& size);
    template<typename T>
    void    LoadOneLump(const dentry_t& entry, T*& data, u_long& num);

private:
    FileBuffer      buffer;     
};
