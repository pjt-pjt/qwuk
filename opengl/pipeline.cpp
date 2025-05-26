#include "pipeline.h"
#include "GL/glew.h"
#include "graphics.h"
#include "filebuffer.h"
#include <cstdio>
#include <assert.h>


//-- VertexBuffer --------------------------------------------------------------

bool    VertexBuffer::Init()
{
    glGenBuffers(1, &vbo);
    ok = (vbo != 0);
    AddAtributes();
    return ok;
}

void    VertexBuffer::Use()
{
    graphics.BindVBO(vbo);
}

void    VertexBuffer::Destroy()
{
    glDeleteBuffers(1, &vbo);
    vbo = 0;
}

void    VertexBuffer::Unuse()
{
    graphics.BindVBO();
}

void    VertexBuffer::SetData(const void* data, uint32_t size)
{
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void    VertexBuffer::AddAtribute(uint32_t index, uint32_t offset, uint32_t size, uint32_t stride)
{
    attributes.push_back({index, offset, size, stride});
}


//-- Pipeline ------------------------------------------------------------------

bool    Pipeline::Init(const VertexBuffer& buffer)
{
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vbo = buffer.GetVBO();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);

    for (const auto& attr : buffer.GetAttributes()) {
        AddAtribute(attr.index, attr.offset, attr.size, attr.stride);
    }
    AddShaders();
    if (!program.IsOK()) {
        return false;
    }
    glBindVertexArray(0);
    return true;
}

void    Pipeline::Destroy()
{
    program.Destroy();
    glDeleteVertexArrays(1, &vao);
    vao = 0;
    vbo = 0;
}

void    Pipeline::Use()
{
    graphics.BindVAO(vao);
    graphics.BindVBO(vbo);
    program.Use();
}

void    Pipeline::Unuse()
{
    graphics.BindVAO();
    program.Unuse();
}


void    Pipeline::AddAtribute(uint32_t index, uint32_t offset, uint32_t size, uint32_t stride)
{
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (void*)(offset * sizeof(float)));
}
