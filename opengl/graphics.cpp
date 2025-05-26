#include "graphics.h"
#include "GL/glew.h"
#include <cstdio>


#define MAX_EXTURE_UNITS 16


bool CheckOK()
{
    bool        ok = true;
    uint32_t    err = glGetError();
    while (err != GL_NO_ERROR) {
        printf("GL Error %d\n", err);
        err = glGetError();
        ok = false;
    }
    return ok;
}


bool    Graphics::Init()
{
    currentTextures.fill(-1);

    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    glEnable(GL_DEPTH_TEST);

    return CheckOK();
}

void    Graphics::Clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void    Graphics::SetViewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

void    Graphics::UseProgram(int program/*  = 0 */)
{
    if (currentProgram != program) {
        glUseProgram(program);
        ++bindsCount;
        currentProgram = program;
    }
    CheckOK();
}

void    Graphics::BindVAO(int vao/*  = 0 */)
{
    if (currentVAO != vao) {
        glBindVertexArray(vao);
        ++bindsCount;
        currentVAO = vao;
        currentVBO = 0;
    }
    CheckOK();
}

void    Graphics::BindVBO(int vbo/*  = 0 */)
{
    if (currentVBO != vbo) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        ++bindsCount;
        currentVBO = vbo;
    }
    CheckOK();
}

void    Graphics::DrawTrangles(DrawMode mode, uint32_t first, uint32_t count)
{
    glDrawArrays(mode == TrinangleFan ? GL_TRIANGLE_FAN : GL_TRIANGLES, first, count);
}

void    Graphics::GenTextures(int count, uint32_t* textureIds)
{
    glGenTextures(count, textureIds);
}

int    Graphics::GenTexture()
{
    GLuint id;
    glGenTextures(1, &id);
    return id;
}

bool    Graphics::CreateTexture(int textureId, const Texture& texture, FilterMode min, FilterMode mag, MipMapMode mipmap)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    GLenum  format = (texture.components == 4) ? GL_RGBA : GL_RED;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, format, GL_UNSIGNED_BYTE, texture.data);
    SetTextureFilters(min, mag, mipmap);
    if (mipmap != MipOff) {
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    BindTexture(currentTextureUnit);
    return CheckOK();
}

bool    Graphics::ChangeTexture(int textureId, const Texture& texture, FilterMode min, FilterMode mag, MipMapMode mipmap)
{
    glBindTexture(GL_TEXTURE_2D, textureId);
    if (texture.data != nullptr) {
        // Change the contents also
    }
    SetTextureFilters(min, mag, mipmap);
    BindTexture(currentTextureUnit);
    return CheckOK();
}

void    Graphics::BindTexture(int unit, int textureId/*  = -1 */)
{
    if (currentTextures[unit] != textureId) {
        if (currentTextureUnit != unit) {
            glActiveTexture(GL_TEXTURE0 + unit);
            ++bindsCount;
            currentTextureUnit = unit;
        }
        glBindTexture(GL_TEXTURE_2D, textureId == -1 ? 0 : textureId);
        ++bindsCount;
        currentTextures[unit] = textureId;
    }
    CheckOK();
}

void    Graphics::DeleteTextures(int count, uint32_t *textureIds)
{
    glDeleteTextures(count, textureIds);
}

void    Graphics::DeleteTexture(uint32_t textureId)
{
    glDeleteTextures(1, &textureId);
}

void    Graphics::SetTextureFilters(FilterMode min, FilterMode mag, MipMapMode mipmap)
{

    GLint   minFilter = GL_NEAREST;
    GLint   magFilter = GL_NEAREST;

    if (min == Linear) {
        if (mipmap == MipLinear) {
            minFilter = GL_LINEAR_MIPMAP_LINEAR;
        } else if (mipmap == MipNearest) {
            minFilter = GL_LINEAR_MIPMAP_NEAREST;
        } else {
            minFilter = GL_LINEAR;
        }
    } else {
        if (mipmap == MipLinear) {
            minFilter = GL_NEAREST_MIPMAP_LINEAR;
        } else if (mipmap == MipNearest) {
            minFilter = GL_NEAREST_MIPMAP_NEAREST;
        }
    }
    if (mag == Linear) {
        magFilter = GL_LINEAR;
    }

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
}


Graphics     graphics;