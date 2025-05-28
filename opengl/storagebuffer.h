#pragma once

#include <cstdint>
#include <vector>


class StorageBuffer
{
public:
    bool            Init();
    void            Destroy();
    void            Use(uint32_t layout);
    void            Unuse();
    bool            IsOK() const { return ok; }

    template<typename V>
    void            SetData(const std::vector<V>& data)
    {
        SetData(data.data(), data.size() * sizeof(V));
    }
    void            SetData(const void* data, uint32_t size);

private:
    bool            ok = false;
    uint32_t        ssbo = 0;
    uint32_t        GetSSBO() const
    {
        return ssbo;
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
