#include "bspfile.h"


bool    BSPFile::Init(const char* file)
{
    if (!buffer.Init(file)) {
        return false;
    }
    return LoadLumps();
}

bool    BSPFile::LoadLumps()
{
    dheader_t*  header = buffer.Get<dheader_t>();
    if (header->version != Version) {
        return false;
    }

    try {
        entities = GetEntry<char>(header->entities);
        LoadOneLump<vertex_t>(header->vertices, vertices, numVertices);
        LoadOneLump<node_t>(header->nodes, nodes, numNodes);
        LoadOneLump<dleaf_t>(header->leaves, leaves, numLeaves);
        LoadOneLump<face_t>(header->faces, faces, numFaces);
        LoadOneLump<u_short>(header->lface, faceList, numFaceList);
        LoadOneLump<edge_t>(header->edges, edges, numEdges);
        LoadOneLump<int>(header->ledges, edgeList, numEdgeList);
        LoadOneLump<model_t>(header->models, models, numModels); 
        LoadOneLump<plane_t>(header->planes, planes, numPlanes);
        LoadOneLump<clipnode_t>(header->clipnodes, clipnodes, numClipnodes);
        textures = GetEntry<mipheader_t>(header->miptex);
        LoadOneLump<surface_t>(header->texinfo, texInfos, numTexinfos);
        lightmapsSize = header->lightmaps.size;
        lightmaps = GetEntry<u_char>(header->lightmaps);
    } catch(...) {
        return false;
    }
    return true;    
}

template<typename T>
T*  BSPFile::GetEntry(const dentry_t& entry)
{
    if (entry.size == 0) {
        return nullptr;
    }
    if (entry.offset + entry.size > buffer.Size()) {
        throw;
    }
    return buffer.Get<T>(entry.offset);
}

template<typename T>
void    BSPFile::LoadOneLump(const dentry_t& entry, T*& data, u_long& num)
{
    data = buffer.Get<T>(entry.offset);
    if ((entry.size % sizeof(T)) != 0) {
        throw;
    }
    num = entry.size / sizeof(T);
}
