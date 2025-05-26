#pragma once

#include <cstdint>
#include <array>


#define MAX_EXTURE_UNITS 16


class Graphics
{
public:
    bool Init();
    void BeginDraw()
    {
        bindsCount = 0;
    }
    uint32_t    GetBindsCount() const
    {
        return bindsCount;
    }
    void Clear();
    void SetViewport(int width, int height);

    void UseProgram(int program = 0);
    void BindVAO(int vao = 0);
    void BindVBO(int vbo = 0);

    enum DrawMode { Triangles, TrinangleFan };
    void DrawTrangles(DrawMode mode, uint32_t first, uint32_t count);

    void GenTextures(int count, uint32_t *textureIds);
    int  GenTexture();
    enum FilterMode { Nearest, Linear };
    enum MipMapMode { MipOff, MipNearest, MipLinear };
    struct Texture {
        int     width;
        int     height;
        int     components;
        const void* data;
    };
    bool LoadTexture(int textureId, const char* path, FilterMode min = Nearest, FilterMode mag = Nearest, MipMapMode mipmap = MipOff);
    bool CreateTexture(int textureId, const Texture& texture, FilterMode min = Nearest, FilterMode mag = Nearest, MipMapMode mipmap = MipOff);
    bool ChangeTexture(int textureId, const Texture& texture, FilterMode min = Nearest, FilterMode mag = Nearest, MipMapMode mipmap = MipOff);
    void BindTexture(int unit, int textureId = -1);
    void DeleteTextures(int count, uint32_t *textureIds);
    void DeleteTexture(uint32_t textureId);

private:
    void SetTextureFilters(FilterMode min, FilterMode mag, MipMapMode mipmap);

private:
    int         currentProgram = 0;
    int         currentVAO = 0;
    int         currentVBO = 0;
    int         currentTextureUnit = -1;
    std::array<int,MAX_EXTURE_UNITS>  currentTextures;

    uint32_t    bindsCount = 0;
};


bool CheckOK();


extern Graphics     graphics;