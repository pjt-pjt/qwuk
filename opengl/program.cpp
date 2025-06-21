#include "program.h"
#include "filebuffer.h"
#include "graphics.h"
#include "GL/glew.h"
#include "glm/gtc/type_ptr.hpp"
#include <vector>


bool    Program::Init(const char* vertexShader, const char* fragmentShader)
{
    static char infoLog[512];
    auto CreateShader = [](uint32_t type, const char* shaderPath) -> uint32_t {
        uint32_t shader = glCreateShader(type);
        TextBuffer  source;
        if (shader != 0 && source.Init(shaderPath)) {
            const char* sourceStr = source.Get().c_str();
            glShaderSource(shader, 1, &sourceStr, nullptr);
            glCompileShader(shader);
    
            int  success;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if(success) {
                return shader;
            } else {
                glGetShaderInfoLog(shader, 512, nullptr, infoLog);
                printf("%s shader compilation failed:\n%s\n", (type == GL_VERTEX_SHADER) ? "Vertex" : "Fragment", infoLog);
            }
        }
        return 0;
    };
    auto CreateProgram = [](uint32_t vertexShaderId, uint32_t fragmentShaderId) -> uint32_t {
        uint32_t linkedProgram = glCreateProgram();
        glAttachShader(linkedProgram, vertexShaderId);
        glAttachShader(linkedProgram, fragmentShaderId);
        glLinkProgram(linkedProgram);
    
        int  success;
        glGetProgramiv(linkedProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(linkedProgram, 512, nullptr, infoLog);
            printf ("Shader linking failed:\n%s\n", infoLog);
            return 0;
        }
    
        return linkedProgram;
    };

    uint32_t vertexShaderId = CreateShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fragmentShaderId = CreateShader(GL_FRAGMENT_SHADER, fragmentShader);
    if (vertexShaderId != 0 && fragmentShaderId != 0) {
        program = CreateProgram(vertexShaderId, fragmentShaderId);
        ok = program != 0;
        glDeleteShader(vertexShaderId);
        glDeleteShader(fragmentShaderId);
    }
    return ok;
}

void    Program::Destroy()
{
    glDeleteProgram(program);
    program = 0;
}

void    Program::Use()
{
    graphics.UseProgram(program);
}

void    Program::Unuse()
{
    graphics.UseProgram();
}

void    Program::SetUniformInt(const char* uniform, int32_t value)
{
    glUniform1i(Location(uniform), value);
}

void    Program::SetUniformUInt(const char* uniform, uint32_t value)
{
    glUniform1ui(Location(uniform), value);
}

void    Program::SetUniformFloat(const char* uniform, float value)
{
    glUniform1f(Location(uniform), value);
}

void    Program::SetUniform(const char* uniform, float x, float y, float z)
{
    glUniform3f(Location(uniform), x, y, z);
}

void    Program::SetUniform(const char* uniform, const glm::vec3& value)
{
    glUniform3fv(Location(uniform), 1, glm::value_ptr(value));
}

void    Program::SetUniform(const char* uniform, const glm::mat4& value)
{
    glUniformMatrix4fv(Location(uniform), 1, GL_FALSE, glm::value_ptr(value));
}

int32_t    Program::Location(const char* uniform)
{
    if (locations.count(uniform) == 1) {
        return locations[uniform];
    }
    int32_t location = glGetUniformLocation(program, uniform);
    if (location != -1) {
        locations[uniform] = location;
        return location;
    }
    return -1;
}
