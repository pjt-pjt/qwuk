#pragma once

#include <cstdint>
#include <string>


class FileBuffer
{
public:
    ~FileBuffer()
    {
        delete [] buffer;
    }

    bool Init(const char* filePath);

    template<typename T>
    T*          Get(size_t offset = 0)          { return reinterpret_cast<T*>(buffer + offset); }
    template<typename T>
    const T*    Get(size_t offset = 0) const    { return reinterpret_cast<T*>(buffer + offset); }

    size_t Size() const
    {
        return size;
    }

private:
    char*   buffer = nullptr;
    size_t  size = 0;
};


class TextBuffer
{
public:
    bool Init(const char* filePath);
    const std::string&  Get() const             { return str; }
    const char*         GetCStr() const         { return cstr; }

private:
    std::string     str;
    const char*     cstr;
};
