#pragma once

#include <random>

namespace aiko::asset
{

    using ID = unsigned long long;

    class Asset
    {
    private:
        ID m_id;
        static ID generateID()
        {
            std::random_device rd;
            std::mt19937_64 gen(rd()); // Generator initialized with random_device
            std::uniform_int_distribution<ID> dis;
            return dis(gen); // Generate a random 64-bit number
        }
    public:
        Asset() : m_id(generateID()) { }
        ~Asset() = default;
        ID getID() const { return m_id; }
    };

    class TextAsset : public Asset
    {
    private:
        const char* data;
    public:
        TextAsset(const char* d) : Asset(), data(d) {} // Correctly initializing `data`
        ~TextAsset() = default;
        const char* getData() const { return data; } // Getter for `data`
    };

    class TextureAsset : public Asset
    {
    public:
        TextureAsset() = default;
        ~TextureAsset() = default;
    };

}
