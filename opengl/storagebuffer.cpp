#include "storagebuffer.h"
#include "GL/glew.h"
#include "graphics.h"
#include <cstdio>
#include <assert.h>


//-- StorageBuffer -------------------------------------------------------------

bool    StorageBuffer::Init()
{
    glGenBuffers(1, &ssbo);
    ok = (ssbo != 0);
    return ok;
}

void    StorageBuffer::Use(uint32_t layout)
{
    graphics.BindSSBO(ssbo);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, layout, ssbo);
}

void    StorageBuffer::Destroy()
{
    glDeleteBuffers(1, &ssbo);
    ssbo = 0;
}

void    StorageBuffer::Unuse()
{
    graphics.BindSSBO();
}

void    StorageBuffer::SetData(const void* data, uint32_t size)
{
    graphics.BindSSBO(ssbo);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_STATIC_DRAW);
    graphics.BindSSBO();
    CheckOK();
}

