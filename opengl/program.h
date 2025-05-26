#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include "glm/glm.hpp"


class Program
{
public:
    bool    Init(const char* vertexShader, const char* fragmentShader);
    void    Destroy(); 
    void    Use();
    void    Unuse();
    bool    IsOK() const
    {
        return ok;
    }

    void    SetUniformInt(const char* uniform, int32_t value);
    void    SetUniformUInt(const char* uniform, uint32_t value);
    void    SetUniformFloat(const char* uniform, float value);

    void    SetUniform(const char* uniform, float x, float y, float z);
    void    SetUniform(const char* uniform, const glm::vec3& value);
    void    SetUniform(const char* uniform, const glm::mat4& value);

private:
    int32_t     Location(const char* uniform);

private:
    bool        ok = false;
    uint32_t    program = 0;

    std::unordered_map<std::string,int32_t>     locations;
};
