#include "filebuffer.h"
#include <fstream>
#include <sstream>


bool    FileBuffer::Init(const char* filePath)
{
    std::ifstream   ifs(filePath, std::ios_base::binary);
    if (!ifs.good()) {
        return false;
    }

    ifs.seekg(0, std::ios_base::end);
    size = ifs.tellg();
    ifs.seekg(0, std::ios_base::beg);

    delete [] buffer;
    buffer = new char[size];

    ifs.read(buffer, size);
    if (!ifs.good()) {
        return false;
    }
    ifs.close();
    return true;
}

bool    TextBuffer::Init(const char* filePath)
{
    std::ifstream   ifs(filePath);
    if (!ifs.good()) {
        return false;
    }
    str.clear();

    std::stringstream   sstr;
    sstr << ifs.rdbuf();
    str = sstr.str();
    cstr = str.c_str();
    return sstr.good();
}
