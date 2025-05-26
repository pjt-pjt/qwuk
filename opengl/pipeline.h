#pragma once

#include "program.h"
#include <cstdint>
#include <vector>


class VertexBuffer
{
public:
    bool            Init();
    void            Destroy();
    void            Use();
    void            Unuse();
    bool            IsOK() const { return ok; }

    virtual void    AddAtributes() = 0;

    template<typename V>
    void            SetData(const std::vector<V>& vertices)
    {
        SetData(vertices.data(), vertices.size() * sizeof(V));
    }
    void            SetData(const void* data, uint32_t size);

protected:
    void            AddAtribute(uint32_t index, uint32_t offset, uint32_t size, uint32_t stride);

private:
    bool            ok = false;
    uint32_t        vbo = 0;
    uint32_t        GetVBO() const
    {
        return vbo;
    }
    struct Attribute
    {   
        uint32_t    index;
        uint32_t    offset;
        uint32_t    size;
        uint32_t    stride;
    };
    std::vector<Attribute>  attributes;
    const std::vector<Attribute>& GetAttributes() const
    {
        return attributes;
    }
    friend class Pipeline;
};


class Pipeline
{
public:
    bool            Init(const VertexBuffer& buffer);
    void            Destroy();
    void            Use();
    void            Unuse();
    bool            IsOK() const { return ok; }
    Program&        GetProgram() { return program; }

protected:
    virtual void    AddShaders() = 0;

protected:
    Program         program;

private:
    void            AddAtribute(uint32_t index, uint32_t offset, uint32_t size, uint32_t stride);

private:
    bool            ok = false;
    uint32_t        vao = 0;
    uint32_t        vbo = 0;
};
