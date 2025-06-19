#pragma once

#include <memory>


class Entities
{
public:
    bool    Init(const char* entities);

private:
    std::unique_ptr<char>   strings;
};
